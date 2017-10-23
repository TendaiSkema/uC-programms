/*
 * NeuralNetwork.c
 *
 * Created: 07.10.2017 00:03:54
 *  Author: Rondof Tendai
 */ 
 
 #include <avr/io.h>
 #include <math.h>
 #include <stdlib.h>

struct NeuralNet
{
	unsigned char neurons[8];						//define the neurons
	double weights[8];								// the weights of each neuron
	double alpha;									//the sum of all inputs times the weights
	double Yhat;									//answer of the neural network
	double Y;
	double LR;
};

unsigned int learnd_in_pc = 0;

unsigned int get_learnd_pc()
{
	return learnd_in_pc;
}

void fead_forward(struct NeuralNet *NN1)
{
	 //reset values
	 NN1->Yhat = 0;
	 NN1->alpha = 0;
	 
	 //compute potential
	 for (unsigned char index = 0;index < 8;index++)
	 {
		 NN1->alpha += ((NN1->neurons[index])*(NN1->weights[index]));			//summ of the neuron values times the weights of each neuron
	 }
	 //compute sigmoid function
	 NN1->Yhat =(1/(1+exp(-1*NN1->alpha)));							// the final answer (sigmoid (1/(1+e^-z)))
}

 void back_propagation(struct NeuralNet *NN1)
 {
	 double delta = (
	 -(((NN1->Y)-(NN1->Yhat))/255)*					// divide Y and Yhat with 255 to make it to the right value
	 (exp((NN1->alpha))/pow( 1+exp( (NN1->alpha) ),2))	// multiply with the dreifediv of sigmoid function
	 );
	 for (unsigned char i=0;i<8;i++)
	 {
		 (NN1->weights[i]) -= (NN1->LR)*(delta*(NN1->neurons[i]));				//redifine weights
	 }
 }

void train(struct NeuralNet *NN1,unsigned int epochs,unsigned int iterations)
{
	 for (unsigned int epoch = 1; epoch <= epochs; epoch++)
	 {
		 for (unsigned int iteration = 0; iteration < iterations; iteration++)
		 {
			 fead_forward(NN1);
			 (NN1->Yhat)*=255;
			 
			 //bakpropagation
			 back_propagation(NN1);			
		 }
	 }
	 learnd_in_pc = 0;
}