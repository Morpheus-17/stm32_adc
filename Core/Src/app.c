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
//ADC�� ���� �Է��ϸ� �ش� ���� �� ������ �µ��� ã�� ��ȯ�Ѵ�
float lookUpTemperature(uint16_t inValue){
	float Vcc = 3.3;							// Vcc ��������
	float R1 = 10000; 						// Pull-up ���װ�
	float Vo; 										// Thermistor ����� ���а�
	float R2; 										// Thremistor ���װ�
	float temperature = 0.0;			// Thermistor ���� �µ�

	// ADC ������ ���� ������ ��� (12Bit�� ADC�� ���)
	Vo = ((float)inValue*Vcc) / 4095.0;

	// Thermistor Resistance
	R2 = R1 / ((Vcc / Vo) - 1.0);

	// Resistance Table ���� R2�� �ش��ϴ� ��ġ�� ã��
	for(int i=0; i<rcColumnlSize-1; i++){
		if(R2<=resistances[i] && R2>resistances[i+1]){
			// ���� ������
			float slope = (temperatures[i+1] - temperatures[i]) / (resistances[i+1] - resistances[i]);
			temperature = temperatures[i] + slope * (R2 - resistances[i]);
			break;
		}
	}

	return temperature;
}

// ��ġ �ڵ� ���
extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;

double kalmanFilter(double measurement){
	//Kalamn filter setup
	static double P =1.0;
	static const double varP = 0.0001; // pow(0.01, 2) //����ġ ���� ���� �����ӵ��� ������
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
	static uint16_t buffer[100]; 	// 100���� ������ ����
	static uint8_t 	index=0;				// ������ ���� ��ġ
	static uint32_t sumValue;
	static uint8_t isFirst = 1;

	//ó�� ȣ��
	if(isFirst){
		isFirst = 0;
		for(int i=0; i<100; i++){
			buffer[i]=inValue;
		sumValue=inValue*100;
		}
	}

	sumValue -= buffer[index];	// ���� ���� ����
	buffer[index] = inValue;		// ���ο� �� ������Ʈ
	sumValue += buffer[index];	// ���ο� ���� ����
	index++;
	index%=100;
	return sumValue / 100;
}

void adcPollingConvert(){
	//Resolution 12bit�̱� ������ 3��Ʈ�� ����� ���� ������ ���⼭�� 2byte�� ����Ѵ�.
	uint16_t adcValue;

	//ADC ��ȯ ����
	HAL_ADC_Start(&hadc1);

	//ADC ���� ���
	HAL_ADC_PollForConversion(&hadc1, 10); // ���Ḧ ����ϴ� Loop(Block) ����

	//�� �б�
	adcValue = HAL_ADC_GetValue(&hadc1);

	//ADC ����
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
		//Polling ����� ADC ��ȯ�� ���ؼ� �˾ƺ���
		adcPollingConvert();

	}
}
