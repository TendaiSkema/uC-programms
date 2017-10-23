/*
 * NeuralNetwork.h
 *
 * Created: 07.10.2017 00:25:57
 *  Author: Gresch Marco
 */ 


#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_

struct NeuralNet
{
	unsigned char neurons[8];						//define the neurons
	double weights[8];								// the weights of each neuron
	double alpha;									//the sum of all inputs times the weights
	double Yhat;									//answer of the neural network
	double Y;
	double LR;
};

unsigned int get_learnd_pc();
void fead_forward(struct NeuralNet *NN1);
void back_propagation(struct NeuralNet *NN1);
void train(struct NeuralNet *NN1,unsigned int epochs,unsigned int iterations);



#endif /* 1HNEURALNETWORK_H_ */