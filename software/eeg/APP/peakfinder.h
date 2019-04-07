// peakfinder.h
#ifndef PEAKFINDER_H
#define PEAKFINDER_H

#define EPS 2.2204e-16

//given a data array of adc samples set peakInds to be an array with the indices of the maximum peaks
//in the data array and also return the number of peaks found in the data array
int peakFinder(int * data, int data_size, int * peakInds);

//sets the output array to contain the derivative of the elements in the input array
//therefore the size of the output array will be one element shorter than the input array
void diff (int * input, int * output, int input_size);

//sets the output array to contain the product of the the elements in arrayA and arrayB
//therefore the size of the output array will be equal to the size of the input array
void arrayProduct(int * arrayA, int * arrayB, int * output, int input_size);

//return the index of the minimum element in the data array
int min(int * data, int data_size);

//return the index of the maximum element in the data array
int max(int * data, int data_size);

//checks if the elements in the inputdata array are less than the threshold value and if so,
//then the index of that element is added to the indices array.
//This function also returns the number of elements in the indices array.
int indicesLessThanThreshold(int * inputdata, int in_size, float threshold, int * indices);

//sets the value of the elements in the outputdata array to be equal to the elements in the
//inputdata array at the indices of the indices array
//ex. outputdata[i] = inputdata[indices[i]] for every element in the indices array
void selectElements(int * inputdata, int * indices, int indices_size, int * outputdata);

#endif
