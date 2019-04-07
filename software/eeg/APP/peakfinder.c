#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "peakfinder.h"

int peakFinder(int * data, int data_size, int * peakInds)
{
	//find the minimum value of the data array
    int minIdx = min(data, data_size);
    //find the maximum value of the data array
    int maxIndx = max(data, data_size);
    //stores the total number of peaks - used as the return value of the peakFinder function
    int totalNumOfIndices = 0;
    //The amount above surrounding data for a peak to be identified (called sel)
    float sel = 250.0;
    //initialize array that will store the derivative of the data array
    int diffdata[data_size - 1];
    //find the derivative of the data array and store it in diffdata
    diff(data, diffdata, data_size);
    //if a data point is zero, change it to 2.2204e-16
    for(int i = 0; i < data_size; i++){
        if(data[i] == 0)
            data[i] = EPS;
    }
    //get size of derivative array
    int diffdata_size = sizeof(diffdata)/sizeof(int);
    //initialize derivative analysis arrays
    int diff1[diffdata_size - 1];
    int diff2[diffdata_size - 1];
    int diff3[diffdata_size - 1];

    for(int i = 1; i < diffdata_size; i++){
    	//load diff1 with the first 98 elements of the derivative array
        diff1[i-1] = diffdata[i-1];
        //load diff2 with the last 98 elements of the derivative array
        diff2[i-1] = diffdata[i];
    }

    //diff3 is the product of diff1 and diff2 (done to check where the sign of the derivative changes;
    //for example the product will remain positive if both diff1 and diff2 have the same sign)
    arrayProduct(&diff1, &diff2, diff3, diffdata_size - 1);
    //initialize an array that will store all the indices of elements where the derivative changes sign
    //therefore this array will contain the INDICES of all the alternating minimum and maximum peaks in
    //the data array
    int changeInDerivativeIndices[diffdata_size - 1];
    //finds where the derivative changes sign and store those indices in the changeInDerivativeIndices array
    int numOfIndices = indicesLessThanThreshold(diff3, diffdata_size - 1, 0, changeInDerivativeIndices);
    //initialize an array that will store the data at the indices when the derivative changes sign
    //therefore this array will contain the VALUES of all the alternating minimum and maximum peaks in
    //the data array
    int derivChangeData[numOfIndices];
    //fill derivChangeData with the data values at the indices when the derivative changes sign
    selectElements(data, changeInDerivativeIndices, numOfIndices, derivChangeData);
    //len represents the number of potential peaks as determined from a change in derivative check
    int len = numOfIndices;
    //find the index with the smallest value in the data array
    int minMagIdx = min(derivChangeData, numOfIndices);
    //find the smallest value in the data array
    int minMagData = derivChangeData[minMagIdx];
    //since set the value of the minimum peak before the maximum peak to the minimum value in the data array
    int leftMin = minMagData;

    //Only run if there are more than 2 potentials alternating peaks. Note that since end points cannot be peaks,
    //a peak requires two other peaks on its left or right to be considered a peak
    if(len > 2)
    {
        //set the temporary peak magnitude to be equal to the smallest peak in the data array
    	int tempPeakMag = minMagData;
    	//reset the foundPeak flag
        bool foundPeak = false;
        //ii represents the index of a maximum peak
        int ii;
        //array of all the potential maximum and minimum peaks (copy of derivChangeData)
        int potentialPeak[len];
        //array of all the indices of the potential maximum and minimum peaks (copy of changeInDerivativeIndices)
        int potentialPeakIndices[len];
        //fill potentialPeak with derivChangeData & fill potentialPeakIndices with changeInDerivativeIndices
        for(int i = 0; i < numOfIndices; i++){
                potentialPeak[i] = derivChangeData[i];
                potentialPeakIndices[i] = changeInDerivativeIndices[i];
        }
        //find where the first maximum peak is, remember that potentialPeak is filled
        //with alternating maximum and minimum peaks
        if(potentialPeak[0] >= potentialPeak[1])
            //first maximum peak is in the 0th index
        	ii = 0;
        else
        	//first maximum peak is in the 1st index
            ii = 1;
        //since potentialPeak is filled with alternating maximum and minimum peaks
        //maxPeaks represents the number of maximum peaks in potentialPeak
        float maxPeaks = ceil(len/2.0);
        //convert the float maxPeaks to the integer mPeaks
        int mPeaks = (int) maxPeaks;
        //initialize the array that will store the INDICES of the final maximum peaks
        int peakLoc[mPeaks];
        //initialize the array that will store the VALUES of the final minimum peaks
        int peakMag[mPeaks];
        //allocate memory for both peakLoc and peakMag and fill both with zeroes
        memset(peakLoc, 0, mPeaks*sizeof(int));
        memset(peakMag, 0, mPeaks*sizeof(int));
        //initialize an integer to keep count of the number of maximum peaks
        int cInd = 1;
        //initialize an integer to hold the temporary maximum peak index
        int tempLoc;

        //run until all the maximum peaks in the potentialPeak array have analyzed
        while(ii < len)
        {
            //increment the maximum peak index in the potentialPeak array
        	ii = ii + 1;
        	//if peak is found, reset the temp peak magnitude value and the foundPeak flag
            if(foundPeak){
                tempPeakMag = minMagData;
                foundPeak = false;
            }

            //check is potential peak that was larger than tempPeakMag, now check
            //that the peak is bigger than the left minimum peak + sel
        	if( potentialPeak[ii-1] > tempPeakMag && potentialPeak[ii-1] > leftMin + sel )
        	{
            	tempLoc = ii-1;
            	tempPeakMag = potentialPeak[ii-1];
        	}

        	//make sure we don't iterate past the length of our vector
        	if(ii == len)
        		//we assign the last point differently out of the loop
        		break;
        	//move onto the valley (point after the peak)
        	ii = ii+1;

        	//ensure that the peak is sel amount bigger than the right minimum peak
        	if(!foundPeak && tempPeakMag > sel + potentialPeak[ii-1])
            {
        		//we have found a peak
	            foundPeak = true;
	            //set right minimum peak to be the new left minimum peak as we are going
	            //to analyze the next maximum peak in the next iteration
	            leftMin = potentialPeak[ii-1];
	            //set peak index to the temporary peak index integer
	            peakLoc[cInd-1] = tempLoc;
	            //set peak index to the temporary peak value integer
	            peakMag[cInd-1] = tempPeakMag;
	            //increment the count of the number of final maximum peaks by one
	            cInd = cInd+1;
	        }
        	//peak was not sel amount bigger than the right minimum so it is not considered a peak,
        	//therefore set right minimum peak to be the new left minimum peak as we are going
            //to analyze the next maximum peak in the next iteration
        	else if(potentialPeak[ii-1] < leftMin)
            	leftMin = potentialPeak[ii-1];
        }
        //check if the last maximum peak in the array is a peak that was larger than tempPeakMag,
        //and also check that the peak is bigger than the left minimum peak + sel
        if (potentialPeak[len-1] > tempPeakMag && potentialPeak[len-1] > leftMin + sel){
        	//set peak index to the temporary peak index integer
            peakLoc[cInd-1] = len-1;
            //set peak index to the temporary peak value integer
            peakMag[cInd-1] = potentialPeak[len-1];
            //increment the count of the number of final maximum peaks by one
            cInd = cInd + 1;
        }
        else if(!foundPeak && tempPeakMag > minMagData){
        	//set peak index to the temporary peak index integer
            peakLoc[cInd-1] = tempLoc;
            //set peak index to the temporary peak value integer
            peakMag[cInd-1] = tempPeakMag;
            //increment the count of the number of final maximum peaks by one
            cInd = cInd + 1;
        }
        //set the peakInds array to be filled with all the values of the potentialPeakIndices array
        //at the indices in the peakLoc array
        //ex. peakInds[i] = potentialPeakIndices[peakLoc[i]] for every element in the peakLoc array
        if(cInd > 0){
            selectElements(potentialPeakIndices, peakLoc, cInd-1, peakInds);
        }
        //calculate the total number of indices of maximum peaks that were found
        totalNumOfIndices = cInd - 1;

    }
    //return the total number of indices of maximum peaks that were sel amount greater than
    //its neighbouring minimum peaks
    return totalNumOfIndices;
}

//Functions are commented in the "peakfinder.h" file

int min(int * data, int data_size)
{
    int minValue = *data;
    int minIndex = 0;

    for(int i = 0; i < data_size; i++){
        if (*(data + i) < minValue){
            {minValue = *(data + i); minIndex = i;}
        }
    }

    return minIndex;
}

int max(int * data, int data_size)
{
    int maxValue = *data;
    int maxIndex = 0;

    for(int i = 0; i < data_size; i++){
        if (*(data + i) > maxValue){
            {maxValue = *(data + i); maxIndex = i;}
        }
    }

    return maxIndex;
}

void diff (int * input, int * output, int input_size)
{
	for(int i = 1; i < input_size; i++){
		*(output + (i - 1)) = input[i] - input[i-1];
	}
}

void arrayProduct(int * arrayA, int * arrayB, int * output, int arrA_size)
{
    for(int i = 0; i < arrA_size; i++){
		*(output + (i)) = arrayA[i] * arrayB[i];
	}
}

int indicesLessThanThreshold(int * inputdata, int in_size, float threshold, int * indices)
{
    int count = 0;

    for(int i = 0; i < in_size; i++){
        if(inputdata[i] < threshold){
            *(indices + (count)) = i+1;
            count += 1;
        }
    }
    return count;
}

void selectElements(int * inputdata, int * indices, int indices_size, int * outputdata)
{
    for(int i = 0; i < indices_size; i++){
        *(outputdata + (i)) = inputdata[indices[i]];
    }
}
