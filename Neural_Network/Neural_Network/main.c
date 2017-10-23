/*
 * Neural_Network.c
 *
 * Created: 13.10.2017 01:26:05
 * Author : Tendai Rondof
 */ 

#include <avr/io.h>
#include "matrix_math.h"
#include <math.h>

#define u8 unsigned char
#define max_size 8

struct Neural_Net{
	u8 input_length;
 	struct matrix weights[max_size];
	u8 depth;
	u8 output_length;
	double LR;
};

void init_Neural_Net(struct Neural_Net *my_NN,u8 input_length,u8 hidden_neurons_depth[max_size],u8 depth,u8 output_length,double learning_rate)
{
	//init the main values of the Neural net structure
	my_NN->input_length=input_length;
	my_NN->depth=depth;
	my_NN->output_length = output_length;
	my_NN->LR = learning_rate;

	//init the first hidden layer
	my_NN->weights[0].rows = input_length;
	my_NN->weights[0].colums = hidden_neurons_depth[0];
	for (u8 row=0;row < my_NN->weights[0].rows;row++)
	{
		for (u8 column =0; column < my_NN->weights[0].colums;column++)
		{
			my_NN->weights[0].array[row][column] = 0.1;
		}
	}

	//init all layer
	for (u8 depth_value=1;depth_value < my_NN->depth; depth_value++)
	{
		my_NN->weights[depth_value].rows = hidden_neurons_depth[depth_value-1];
		my_NN->weights[depth_value].colums = hidden_neurons_depth[depth_value];
		for (u8 row = 0; row < my_NN->weights[depth_value].rows;row++)
		{
			for (u8 column=0;column < my_NN->weights[depth_value].colums;column++)
			{
				my_NN->weights[depth_value].array[row][column] = 0.1;
			}
		}
	}

	//init output layer
	my_NN->weights[depth].rows = hidden_neurons_depth[depth-1];
	my_NN->weights[depth].colums = output_length;
	for (u8 row=0;row < my_NN->weights[depth].rows;row++)
	{
		for (u8 column =0; column < my_NN->weights[depth].colums;column++)
		{
			my_NN->weights[depth].array[row][column] = 0.1;
		}
	}
}

void sigmoid(struct matrix *alpha,struct matrix *z)
{
	z->rows = alpha->rows;
	z->colums = alpha->colums;
	for (u8 row=0;row < alpha->rows;row++)
	{
		for (u8 column =0; column < alpha->colums;column++)
		{
			z->array[row][column] = (1/(1+exp(-1*alpha->array[row][column])));
		}
	}
}

struct matrix fead_forward(struct matrix *X,struct Neural_Net *my_NN)
{
	struct matrix alpha;
	struct matrix z;

	alpha = multiply(X,&my_NN->weights[0]);
	sigmoid(&alpha,&z);
	
 	for (unsigned char layer =1;layer < my_NN->depth;layer++)
 	{
	 	alpha = multiply(&z,&my_NN->weights[layer]);
		sigmoid(&alpha,&z);
 	}
	struct matrix yHat;
	alpha = multiply(&z,&my_NN->weights[my_NN->depth]);
	sigmoid(&alpha,&yHat);
	return yHat;
}



void train(struct Neural_Net *my_NN,struct matrix *X,struct matrix *Y)
{
	struct matrix yHat = fead_forward(X,my_NN);

}

int main(void)
{
	DDRH = 0xFF;
	PORTH = 0;

	struct Neural_Net NN;
	u8 hidden_neurons[] = {2,3,2};
	init_Neural_Net(&NN,8,hidden_neurons,3,8,0.01);

	struct matrix X;
	X.rows=5;
	X.colums=8;
	for (u8 row=0;row<X.rows;row++)
	{
		X.array[row][row] = 1;
	}

	fead_forward(&X,&NN);
}

