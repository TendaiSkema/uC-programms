/*
 * 1H_Network.c
 *
 * Created: 06.10.2017 11:13:46
 * Author : e6Rondof
 */ 

#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "1HNeuralNetwork.h"

#define S1 (PINE & (1<<PINE4))
#define S2 (PINE & (1<<PINE5))

#define RGB_RED (PORTH |= (1<<PH3))
#define RGB_GREEN (PORTH |= (1<<PH4))
#define RGB_BLUE (PORTH |= (1<<PH5))
#define RGB_RED_of (PORTH &= ~(1<<PH3))
#define RGB_GREEN_of (PORTH &= ~(1<<PH4))
#define RGB_BLUE_of (PORTH &= ~(1<<PH5))

struct NeuralNet NN;									//init of global Neural network (because of errors)
unsigned int epochs = 20;								//how many tymes to repeat all iteratins
unsigned int iterations = 2000;							//how many training iterrations in one epoch
unsigned int timer = 0;
unsigned char yhat;

void init_timer()
{
	TCCR1B = (1<<CS10);
	TIMSK1 = (1<<TOIE1);
}

ISR(TIMER1_OVF_vect)
{
	timer++;
}

void blink(unsigned char times)
{
	for (int i = 0; i < times; i++)
	{
		PORTA = 0x00;
		timer =0;
		while (timer<5);
		PORTA = 0xFF;
		timer =0;
		while (timer<5);	
	}
}
void wait(int length)
{
	timer = 0;
	while (timer<length);
}

int main(void)
{
	DDRA = 0xFF;										//set led Port to output
	DDRC = 0;											//set PORT C as Input
	DDRE = 0;
	DDRH = 0xFF;										//set rgb led port to output

	PORTH = 0;											//turn off rgb led's
	PORTA = 0xFF;										//turn off led's

	init_timer();
	sei();

	//init Neural Network vars
	for (unsigned char i=0;i<8;i++)
	{
		NN.weights[i]=0.1;								//define all weights
	}
	
	NN.alpha = 0;										//the summ of all inputs times the weights
	NN.Yhat = 0;										//answer of the neural network
	NN.Y = 0;											//right answer
	NN.LR = 0.1;										//Learning rate

	blink(4);
	RGB_GREEN;
    while (1) 
    {
		if (S1)															//wait for taste 1
		{
			RGB_RED;
			//read hardware input
			for (unsigned char neuron = 0; neuron<8;neuron++)
			{
				NN.neurons[neuron] = (PINC&(1<<neuron))>>neuron;		//neurons are equal to inputs
			}

			fead_forward(&NN);											// normal fead forward 
			NN.Yhat*=255;
			yhat = NN.Yhat;
			PORTA = ~yhat;											// make a u8 value from double Yhat

			wait(500);
			blink(4);

			RGB_GREEN_of;
			RGB_RED_of;

			while(1)													//wait for user feedback
			{
				RGB_BLUE;
				PORTA = ~PINC;
				if (S1)											//if s1 is pressed its wrong
				{
					RGB_BLUE_of;
					RGB_GREEN;
					RGB_RED;

					NN.Y=PINC;											//read the right value from Port C
					train(&NN,epochs,iterations);						// train

					RGB_GREEN_of;
					RGB_RED_of;
					RGB_BLUE;

					NN.Yhat*=255;
					yhat = NN.Yhat;
					PORTA = ~yhat;
					wait(500);											// make a u8 value from double Yhat
					break;
				}
				else if (S2)										//if s2 is pressed its true and the programm goes on
				{
					break;
				}
			}
			blink(4);
			RGB_BLUE_of;
			RGB_RED_of;
			RGB_GREEN;
		}
    }
}













// unsigned char neurons[8];//define the neurons
// double weights[8] = {							//init of the waights
// 	(0.5),							//init of the waights
// 	(0.5),							//init of the waights
// 	(0.5),							//init of the waights
// 	(0.5),							//init of the waights
// 	(0.5),							//init of the waights
// 	(0.5),							//init of the waights
// 	(0.5),							//init of the waights
// 	(0.5)							//init of the waights
// };
// double alpha = 0;								//the summ of all inputs times the weights
// double Yhat;									//answer of the neural network
// double Y;
// double LR = 1;										//the right answer








// void fead_forward()
// {
// 	//reset values
// 	Yhat = 0;
// 	alpha = 0;
//
// 	//compute potential
// 	for (unsigned char index = 0;index < 8;index++)
// 	{
// 		alpha += (neurons[index]*weights[index]);			//summ of the neuron values times the weights of each neuron
// 	}
// 	//compute sigmoid function
// 	Yhat =(1/(1+exp(-1*alpha)));							// the final answer (sigmoid (1/(1+e^-z)))
// }

// void back_propagation()
// {
// 	double delta = (
// 					-((Y-Yhat)/255)*					// divide Y and Yhat with 255 to make it to the right value
// 					(exp(alpha)/pow( 1+exp( alpha ),2))	// multiply with the dreifediv of sigmoid function
// 					);
// 	for (unsigned char i=0;i<8;i++)
// 	{
// 		weights[i] -= LR*(delta*neurons[i]);				//redifine weights
// 	}
// }

// void train()
// {
// 	PORTH ^= (1<<PH4)|(1<<PH3);
// 	for (unsigned int epoch = 1; epoch <= epochs; epoch++)
// 	{
// 		PORTA = 0xFF;
// 		for (unsigned int iteration = 0; iteration < iterations; iteration++)
// 		{
// 			fead_forward();
// 			Yhat*=255;
//
// 			//bakpropagation
// 			back_propagation();
//
// 			if (iteration%(iterations/8)==0)
// 			{
// 				PORTA= ~(((~PORTA)<<1)+1);
// 			}
// 		}
//
// 		PORTA = 0xFF;											//just for human eyes
// 		timer=0;											//just for human eyes
// 		while(timer<15);
// 		PORTA = ~epoch;
// 		timer=0;											//just for human eyes
// 		while(timer<15);									//just for human eyes
// 		PORTA = 0xFF;											//just for human eyes
// 		timer=0;											//just for human eyes
// 		while(timer<15);									//just for human eyes
// 	}
//
// 	PORTA = 0xFF;											//just for human eyes
// 	PORTH ^= (1<<PH4)|(1<<PH3);
// 	timer=0;											//just for human eyes
// 	while(timer<15);
// 	PORTA = 0;
// 	timer=0;											//just for human eyes
// 	while(timer<15);									//just for human eyes
// 	PORTA = 0xFF;											//just for human eyes
// 	timer=0;											//just for human eyes
// }