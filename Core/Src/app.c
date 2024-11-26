/*
 * app.c
 *
 *  Created on: Nov 25, 2024
 *      Author: blueb
 */

#include "app.h"
#include "uart.h"
#include "NTC_Thermistor.h"
#include <stdio.h>

const int rcColumnlSize = sizeof(resistances)/sizeof(resistances[0]);
//ADC의 값을 입력하면 해당 저항 값 구간의 온도를 찾아 반환한다
float lookUpTemperature(uint16_t inValue){
	float Vcc = 3.3;							// Vcc 전원전압
	float R1 = 10000; 						// Pull-up 저항값
	float Vo; 										// Thermistor 양단의 전압값
	float R2; 										// Thremistor 저항값
	float temperature = 0.0;			// Thermistor 계산된 온도

	// ADC 값으로 부터 전압을 계산 (12Bit의 ADC인 경우)
	Vo = ((float)inValue*Vcc) / 4095.0;

	// Thermistor Resistance
	R2 = R1 / ((Vcc / Vo) - 1.0);

	// Resistance Table 에서 R2에 해당하는 위치를 찾음
	for(int i=0; i<rcColumnlSize-1; i++){
		if(R2<=resistances[i] && R2>resistances[i+1]){
			// 선형 보간법
			float slope = (temperatures[i+1] - temperatures[i]) / (resistances[i+1] - resistances[i]);
			temperature = temperatures[i] + slope * (R2 - resistances[i]);
			break;
		}
	}

	return temperature;
}

// 장치 핸들 등록
extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;

double kalmanFilter(double measurement){
	//Kalamn filter setup
	static double P =1.0;
	static const double varP = 0.0001; // pow(0.01, 2) //가중치 작을 수록 반응속도가 빠르다
	static const double R = 0.25; 			// pow(0.5, 2) //
	static double K = 1.0;
	static double X = 20.0;

	// Kalman Simple Filter
	P = P + varP;
	K = P / (P + R);
	X = (K * measurement) + (1- K) * X;
	P = (1 -K ) * P;
	return X;
}

uint16_t movingMeanFilter(uint16_t inValue){
	static uint16_t buffer[100]; 	// 100개의 샘플을 저장
	static uint8_t 	index=0;				// 샘플의 저장 위치
	static uint32_t sumValue;
	static uint8_t isFirst = 1;

	//처음 호출
	if(isFirst){
		isFirst = 0;
		for(int i=0; i<100; i++){
			buffer[i]=inValue;
		sumValue=inValue*100;
		}
	}

	sumValue -= buffer[index];	// 옛날 값을 빼줌
	buffer[index] = inValue;		// 새로운 값 업데이트
	sumValue += buffer[index];	// 새로운 값을 더함
	index++;
	index%=100;
	return sumValue / 100;
}

void adcPollingConvert(){
	//Resolution 12bit이기 때문에 3비트를 사용할 수도 있으나 여기서는 2byte로 사용한다.
	uint16_t adcValue;

	//ADC 변환 시작
	HAL_ADC_Start(&hadc1);

	//ADC 종료 대기
	HAL_ADC_PollForConversion(&hadc1, 10); // 종료를 대기하는 Loop(Block) 구간

	//값 읽기
	adcValue = HAL_ADC_GetValue(&hadc1);

	//ADC 중지
	HAL_ADC_Stop(&hadc1);

	float temp1 = lookUpTemperature(adcValue);
	float temp2 = lookUpTemperature(movingMeanFilter(adcValue));
	float temp3 = lookUpTemperature(kalmanFilter(adcValue));


//	printf("%d\t%d\t%d\n ",adcValue, movingMeanFilter(adcValue), (int)(kalmanFilter(adcValue)));
	printf("%f\t%f\t%f\n", temp1, temp2, temp3);

	HAL_Delay(50);
}

void app(){

	initUart(&huart2);

	while(1){
		//Polling 방식의 ADC 변환에 대해서 알아본다
		adcPollingConvert();

	}
}
