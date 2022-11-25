#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MAXN 512

int rank, nprocs;
int i, j;
int chunk;
MPI_Datatype Complextype;

typedef struct { float r; float i; } complex;
static complex ctmp;

complex data1[MAXN][MAXN];
complex data2[MAXN][MAXN];
complex data3[MAXN][MAXN];


#define C_SWAP(a,b) {ctmp=(a);(a)=(b);(b)=ctmp;}

void fft1d(complex *r, int n, int isign)
{
	int     m, i, i1, j, k, i2, l, l1, l2;
	float   c1, c2, z;
	complex t, u;

	if (isign == 0) return;

	/* Do the bit reversal */
	i2 = n >> 1;
	j = 0;
	for (i = 0; i<n - 1; i++) {
		if (i < j)
			C_SWAP(r[i], r[j]);
		k = i2;
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	/* m = (int) log2((double)n); */
	for (i = n, m = 0; i>1; m++, i /= 2);

	/* Compute the FFT */
	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for (l = 0; l<m; l++) {
		l1 = l2;
		l2 <<= 1;
		u.r = 1.0;
		u.i = 0.0;
		for (j = 0; j<l1; j++) {
			for (i = j; i<n; i += l2) {
				i1 = i + l1;

				/* t = u * r[i1] */
				t.r = u.r * r[i1].r - u.i * r[i1].i;
				t.i = u.r * r[i1].i + u.i * r[i1].r;

				/* r[i1] = r[i] - t */
				r[i1].r = r[i].r - t.r;
				r[i1].i = r[i].i - t.i;

				/* r[i] = r[i] + t */
				r[i].r += t.r;
				r[i].i += t.i;
			}
			z = u.r * c1 - u.i * c2;

			u.i = u.r * c2 + u.i * c1;
			u.r = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		if (isign == -1) /* FWD FFT */
			c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}

	/* Scaling for inverse transform */
	if (isign == 1) {       /* IFFT*/
		for (i = 0; i<n; i++) {
			r[i].r /= n;
			r[i].i /= n;
		}
	}
}

/*Read input files*/
void file_read()
{
	FILE *f1, *f2; /*open file descriptor */
	f1 = fopen("1_im1", "r");
	f2 = fopen("1_im2", "r");
	for (i = 0; i<MAXN; i++)
	{
		for (j = 0; j<MAXN; j++)
		{
			//printf("%e", data1[i][j].r);
			fscanf(f1, "%e", &data1[i][j].r);
			fscanf(f2, "%e", &data2[i][j].r);
		}
	}
	fclose(f1);
	fclose(f2);
}

/*Write Output File*/
void file_write()
{
	FILE *f1; /*open file descriptor */
	f1 = fopen("output", "w+");
	for (i = 0; i<MAXN; i++)
	{
		for (j = 0; j<MAXN; j++)
		{
			fprintf(f1, "%1.6e  ", data3[i][j].r);
		}
		fprintf(f1, "\n");
	}
	fclose(f1);
}

int main(int argc, char* argv[])
{
	double etstart, etstop;  /* Elapsed times using MPI */
	double t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12;
	int p;
	
	file_read();
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	chunk = MAXN / nprocs;
	
	if (rank == 0)
		t1 = MPI_Wtime();
	
	/*Distribution of data1 and data2*/
	if (rank == 0)
	{
		for (p = 1; p < nprocs; p++)
		{
			MPI_Send(&data1[p*chunk][0], chunk*MAXN, MPI_COMPLEX, p, 0, MPI_COMM_WORLD);
			MPI_Send(&data2[p*chunk][0], chunk*MAXN, MPI_COMPLEX, p, 0, MPI_COMM_WORLD);
		}
	}
	else
	{
		MPI_Recv(&data1[0][0], chunk*MAXN, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&data2[0][0], chunk*MAXN, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	
	if (rank == 0)
		t2 = MPI_Wtime();
	
	/*1D FFT Row wise*/
	for (i = 0; i<chunk; i++)
	{
		fft1d(data1[i], MAXN, -1);
		fft1d(data2[i], MAXN, -1);
	}
	
	if (rank == 0)
		t3 = MPI_Wtime();
	
	/*Collecting data1 & data2*/
	if (rank == 0)
	{
		for (p = 1; p < nprocs; p++)
		{
			MPI_Recv(&data1[p*chunk][0], chunk*MAXN, MPI_COMPLEX, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&data2[p*chunk][0], chunk*MAXN, MPI_COMPLEX, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	else
	{
		MPI_Send(&data1[0][0], chunk*MAXN, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&data2[0][0], chunk*MAXN, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD);
	}
	
	if (rank == 0)
		t4 = MPI_Wtime();
	
	/*Transpose*/
	for (j = 0; j < MAXN; j++)
	{
		for (i = j; i < MAXN; i++)
		{
			C_SWAP(data1[j][i], data1[i][j]);
			C_SWAP(data2[j][i], data2[i][j]);
		}
	}
	
	if (rank == 0)
		t5 = MPI_Wtime();
	
	/*Again Distribution of data1 & data2*/
	if (rank == 0)
	{
		for (p = 1; p < nprocs; p++)
		{
			MPI_Send(&data1[p*chunk][0], chunk*MAXN, MPI_COMPLEX, p, 0, MPI_COMM_WORLD);
			MPI_Send(&data2[p*chunk][0], chunk*MAXN, MPI_COMPLEX, p, 0, MPI_COMM_WORLD);
		}
	}
	else
	{
		MPI_Recv(&data1[0][0], chunk*MAXN, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&data2[0][0], chunk*MAXN, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	if (rank == 0)
		t6 = MPI_Wtime();
	
	/*1D FFT Row wise (Transposed Column)*/
	for (i = 0; i<chunk; i++)
	{
		fft1d(data1[i], MAXN, -1);
		fft1d(data2[i], MAXN, -1);
	}
	
	/*Point Multiplication*/
	for (i = 0; i < chunk; i++)
	{
		for (j = 0; j < MAXN; j++)
		{
			data3[i][j].r = data1[i][j].r * data2[i][j].r - data1[i][j].i * data2[i][j].i;
			data3[i][j].i = data1[i][j].r * data2[i][j].i + data1[i][j].i * data2[i][j].r;
		}
	}
	
	/*1D IFFT Row wise on Output Matrix*/
	for (i = 0; i<chunk; i++)
	{
		fft1d(data3[i], MAXN, 1);
	}

	if (rank == 0)
		t7 = MPI_Wtime();
	
	/*Collecting data3*/
	if (rank == 0)
	{
		for (p = 1; p < nprocs; p++)
		{
			MPI_Recv(&data3[p*chunk][0], chunk*MAXN, MPI_COMPLEX, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	else
	{
		MPI_Send(&data3[0][0], chunk*MAXN, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD);
	}
	
	if (rank == 0)
		t8 = MPI_Wtime();
	
	/*Transpose*/
	for (j = 0; j < MAXN; j++)
	{
		for (i = j; i < MAXN; i++)
		{
			C_SWAP(data3[i][j], data3[j][i]);
		}
	}

	if (rank == 0)
		t9 = MPI_Wtime();
	
	/*Distribution of data3*/
	if (rank == 0)
	{
		for (p = 1; p < nprocs; p++)
		{
			MPI_Send(&data3[p*chunk][0], chunk*MAXN, MPI_COMPLEX, p, 0, MPI_COMM_WORLD);
		}
	}
	else
	{
		MPI_Recv(&data3[0][0], chunk*MAXN, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	if (rank == 0)
		t10 = MPI_Wtime();
	
	/*1D IFFT Row wise(Transposed Column) */
	for (i = 0; i<chunk; i++)
	{
		fft1d(data3[i], MAXN, 1);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	if (rank == 0)
		t11 = MPI_Wtime();
	
	/*Collecting data3*/
	if (rank == 0)
	{
		for (p = 1; p < nprocs; p++)
		{
			MPI_Recv(&data3[p*chunk][0], chunk*MAXN, MPI_COMPLEX, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	else
	{
		MPI_Send(&data3[0][0], chunk*MAXN, MPI_COMPLEX, 0, 0, MPI_COMM_WORLD);
	}
	
	if (rank == 0)
		t12 = MPI_Wtime();
	
	if (rank == 0)
	{
		file_write();
		double c1 = (t2-t1)+(t4-t3)+(t6-t5)+(t8-t7)+(t10-t9)+(t12-t11);
		double c2 = (t3-t2)+(t5-t4)+(t7-t6)+(t9-t8)+(t11-t10);
		printf("Computation Cost: %f msecs\n",c2*1000);
		printf("Communication Cost: %f msecs\n",c1*1000);
	}
	MPI_Finalize();
}

