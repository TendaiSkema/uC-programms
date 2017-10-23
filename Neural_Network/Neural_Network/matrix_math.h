/*
 * matrix_math.h
 *
 * Created: 12.10.2017 01:50:25
 *  Author: Gresch Marco
 */ 


#ifndef MATRIX_MATH_H_
#define MATRIX_MATH_H_

/*
	The Matrix structure has the matrix and it's dimensions.
	it has also a transposed matrix of it self with first have to be initialiesd
	with the Transpose Function.
	This function creats the Transposed matrix and the dimension values
*/
struct matrix{
	double array[10][10];
	unsigned char rows;
	unsigned char colums;
};

enum boolean{false,true};

//init the Transposed matrix of a matrix structure witch was given with a pointer
void Transpose(struct matrix *my_matrix);

//set a matrix structure equal to another
void set_equal(struct matrix *target_m,struct matrix *full_m);

//standart math
struct matrix sub(struct matrix *m1,struct matrix *m2);
struct matrix add(struct matrix *m1,struct matrix *m2);
struct matrix multiply(struct matrix *my_matrix1,struct matrix *my_matrix2);
enum boolean are_equal(struct matrix *m1,struct matrix *m2);
void fill(struct matrix *m1, double value);



#endif /* MATRIX_MATH_H_ */