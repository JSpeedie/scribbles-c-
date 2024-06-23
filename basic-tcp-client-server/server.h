#ifndef __SERVER_H_
#define __SERVER_H_

/* C includes */
#include <stdio.h>
#include <unistd.h>

/* C++ includes */
#include <cstdint> // For int32_t
#include <cstdio> // For sscanf()
#include <fstream> // For std::ifstream
#include <iostream> // For std::ifstream
#include <numeric> // For std::accumulate
#include <string> // For std::string
#include <vector> // For std::vector


std::string msg_data_unavailable = "Data unavailable";
/* The filepath to the sensor data file used by our server */
std::string sensor_data_fp = "sensor_data.txt";
int server_backlog = 10;
/* Set the size of the sliding window used by our server when returning
 * sensor data */
int sensor_data_sliding_window_size = 10;


class SensorDataReader {
	private:
		std::vector<int16_t> slidingWindow;
		size_t cur_index = 0;
		size_t slidingWindowMaxWidth = 1;
		std::ifstream sensorDataFile;

	public:
		SensorDataReader(const char *filepath, int16_t slidingWindowSize) {
			/* Open the sensor data file */
			sensorDataFile = std::ifstream(filepath);
			slidingWindowMaxWidth = slidingWindowSize;
		}

		/** This function is used to determine if the SensorDataReader has
		 * been initialized correctly and is ready for reading. The only
		 * thing that can prevent the reader from being ready is if the
		 * sensor data file it was given does not exist.
		 *
		 * @return true if the reader is ready to read and false if it is not.
		 */
		bool ready() {
			if (sensorDataFile) {
				return true;
			/* If the sensor data file does not exist */
			} else {
				return false;
			}
		}

		/** This function attempts to return the most recent sensor data
		 * filtered through a moving average (i.e. sliding window).
		 *
		 * @return 0 upon success and -1 upon failure. On success it modifies
		 * '&numerator' to contain the running total of the sensor values in
		 * the sliding window, and '&denominator'  to contain the number of
		 * values in the sliding window. Dividing '&numerator' by
		 * '&denominator' will give the caller the moving average.
		 *
		 * The reason the data is returned as a numerator, denominator pair is
		 * so that this data can be sent over a network without experiencing
		 * endianness difficulties with floats/doubles.
		 */
		int getData(int32_t & numerator, int8_t & denominator) {
			std::string fileLine;
			/* Get one line from the file */
			/* If getting one line succeeds */
			if (getline(sensorDataFile, fileLine)) {
				int16_t temp;
				/* Take the line read, parse it as an int16_t, and either
				 * append it to the vector if the vector is not yet at its max
				 * size, or replace the element at the current index if it is */
				sscanf(fileLine.data(), "%hi", &temp);
				if (slidingWindow.size() < slidingWindowMaxWidth) {
					slidingWindow.push_back(temp);
				} else {
					slidingWindow.at(cur_index) = temp;

					/* Increase the current element index, looping back to 0 if
					* needed so that this method will overwrite old values
					* effectively shifting the sliding window */
					cur_index++;
					if (cur_index >= slidingWindow.size()) cur_index = 0;
				}

				/* Sum up all the values from our sliding window for the
				 * numerator, and set the denominator to the number of elements
				 * in the sliding window */
				numerator = \
					std::accumulate(slidingWindow.begin(), slidingWindow.end(), 0);
				denominator = slidingWindow.size();

				return 0;
			/* If getting one line fails, then the data is unavailable */
			} else {
				return -1;
			}
		}

		~SensorDataReader() {
			/* Close the sensor data file */
			sensorDataFile.close();
		}
};

#endif
