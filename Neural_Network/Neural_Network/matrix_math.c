/*
 * matrix_math.c
 *
 * Created: 12.10.2017 01:49:31
 *  Author: Tendai Rondof
 */ 
 struct matrix{
	 double array[10][10];
	 unsigned char rows;
	 unsigned char colums;
 };

 enum boolean {false,true};

 struct matrix multiply(struct matrix *my_matrix1,struct matrix *my_matrix2)
 {
	struct matrix return_matrix;

	 if (my_matrix1->colums == my_matrix2->rows)
	{
		return_matrix.colums = my_matrix2->colums;
		return_matrix.rows = my_matrix1->rows;

		for (unsigned char row1=0;row1<my_matrix1->rows;row1++)
		{
			for (unsigned char column2 =0; column2<my_matrix2->colums;column2++)
			{
				 unsigned char buffer_value =0;
				 for (unsigned char row2=0;row2<my_matrix2->rows;row2++)
				 {
					 buffer_value+= my_matrix1->array[row1][row2]*my_matrix2->array[row2][column2];
				 }
				 return_matrix.array[row1][column2] = buffer_value;
				 
			}
		}
		return return_matrix;
	}
	return return_matrix;
 }

 struct matrix Transpose(struct matrix *my_matrix)
 {
	struct matrix return_matrix;
	return_matrix.rows = my_matrix->colums;
	return_matrix.colums = my_matrix->rows;

	for (unsigned char row = 0; row < my_matrix->rows;row++)
	{
		for (unsigned char column = 0; column < my_matrix->colums;column++)
		{
			return_matrix.array[column][row] = my_matrix->array[row][column];
		}
	}
	return return_matrix;
 }

 void set_equal(struct matrix *target_m,struct matrix *full_m)
 {
	target_m->rows = full_m->rows;
	target_m->colums = full_m->colums;

	for (unsigned char row=0;row < full_m->rows;row++)
	{
		for (unsigned char column=0;column < full_m->colums;column++)
		{
			target_m->array[row][column] = full_m->array[row][column];
		}
	}
 }

struct matrix add(struct matrix *m1,struct matrix *m2)
{
	struct matrix return_matrix;
	if (m1->rows == m2->rows && m1->colums == m2->colums)
	{
		return_matrix.rows = m1->rows;
		return_matrix.colums = m1->colums;

		for (unsigned char row=0;row < m1->rows;row++)
		{
			for (unsigned char column =0; column < m1->colums;column++)
			{
				return_matrix.array[row][column] = m1->array[row][column] + m2->array[row][column];
			}
		}
		return return_matrix;
	}
	return return_matrix;
}

struct matrix sub(struct matrix *m1,struct matrix *m2)
{
	struct matrix return_matrix;
	if (m1->rows == m2->rows && m1->colums == m2->colums)
	{
		return_matrix.rows = m1->rows;
		return_matrix.colums = m1->colums;

		for (unsigned char row=0;row < m1->rows;row++)
		{
			for (unsigned char column =0; column < m1->colums;column++)
			{
				return_matrix.array[row][column] = m1->array[row][column] - m2->array[row][column];
			}
		}
		return return_matrix;
	}
	return return_matrix;
}

enum boolean are_equal(struct matrix *m1,struct matrix *m2)
{
	if (m1->rows == m2->rows && m1->colums == m2->colums)
	{
		for (unsigned char row=0;row < m1->rows;row++)
		{
			for (unsigned char column =0; column < m1->colums;column++)
			{
				if (m1->array[row][column] != m2->array[row][column])
				{
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

void fill(struct matrix *m1, double value)
{
	for (unsigned char row=0;row < m1->rows;row++)
	{
		for (unsigned char column =0; column < m1->colums;column++)
		{
			m1->array[row][column] = value;
		}
	}
}