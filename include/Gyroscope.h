/*
 * Gyroscope.h
 *
 *  Created on: Dec 2, 2014
 *      Author: benling
 */

#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_

#define L3G_Sensitivity_250dps     (float)   114.285f         /*!< gyroscope sensitivity with 250 dps full scale [LSB/dps] */
#define L3G_Sensitivity_500dps     (float)    57.1429f        /*!< gyroscope sensitivity with 500 dps full scale [LSB/dps] */
#define L3G_Sensitivity_2000dps    (float)    14.285f       /*!< gyroscope sensitivity with 2000 dps full scale [LSB/dps] */

void GyroConfig(void);
void GyroReadAngRate(float* pfData);



#endif /* GYROSCOPE_H_ */
