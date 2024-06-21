#ifndef __SERVER_H_
#define __SERVER_H_

/* C includes */
#include <stdio.h>
#include <unistd.h>

/* C++ includes */
#include <array> // For std::array
#include <cstdint> // For int32_t
#include <cstdio> // For sscanf()
#include <fstream> // For ifstream
#include <iostream> // For ifstream
#include <string> // For std::string


std::string msg_data_unavailable = "Data unavailable";
std::string sensor_data_fp = "sensor_data.txt";
int server_backlog = 10;


class SensorDataReader {
	private:
		std::array<short, 10> slidingWindow;
		char cur_index = 0;
		char slidingWindowWidth = 1;
		std::ifstream sensorDataFile;

	public:
		SensorDataReader(const char *filepath) {
			/* Open the sensor data file */
			sensorDataFile = std::ifstream(filepath);
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
		int getData(int32_t & numerator, char & denominator) {
			std::string fileLine;
			/* Get one line from the file */
			/* If getting one line succeeds */
			if (getline(sensorDataFile, fileLine)) {
				sscanf(fileLine.data(), "%hi", &slidingWindow.at(cur_index));

				/* Sum up 'slidingWindowWidth' elements from our
				 * 'slidingWindow' array in reverse (most recent to least),
				 * looping back to the end of the array if needed */
				numerator = 0;
				denominator = slidingWindowWidth;
				char temp_index = cur_index;
				for (int i = 0; i < slidingWindowWidth; i++) {
					numerator += slidingWindow.at(temp_index);
					temp_index--;
					if (temp_index < 0) {
						temp_index = slidingWindow.size() - 1;
					}
				}

				/* If we have not filled our sliding window */
				if (slidingWindowWidth < static_cast<char>(slidingWindow.size())) {
					slidingWindowWidth++;
				}

				/* Increase current element index, looping back to 0 if needed */
				cur_index++;
				if (cur_index >= static_cast<char>(slidingWindow.size())) {
					cur_index = 0;
				}

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
