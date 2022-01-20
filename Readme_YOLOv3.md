---
title: YOLOv3
updated: 2022-01-19 14:40:52Z
created: 2022-01-17 10:20:23Z
latitude: 43.08100000
longitude: -2.50930000
altitude: 0.0000
---

Download from github YOLOv3: ``https://github.com/AlexeyAB/darknet.git``
Download activations weights: 
- windows: ``https://pjreddie.com/media/files/yolov3.weights``
- linux: ``wget https://pjreddie.com/media/files/yolov3.weights `` 

Here and in advance, the guideline is focus on Linux OS.
1. Modify Makefile in root path and uncomment the following line: 
```plain text
# Jetson XAVIER
ARCH= -gencode arch=compute_72,code=[sm_72,compute_72]
```
 2. Create the executable with make command. Go to nutshell, go to terminal and execute ``make`` command.
 3. Copy the set of images to YOLOv3
 4. Compute a single object detection:
```plaintext
./darknet detector test ./cfg/coco.data ./cfg/yolov3.cfg ./yolov3.weights -dont_show -ext_output <data/file_list.txt> results.csv
```

How to include a prefix at the beginning of all sentences: 
```plaintext
ex -sc '%s/^/prefix/|x' file

# In our case
ex -sc '%s/^/bdd100k_images_10k//|x' file_list.txt
```

5. Modify the image.c and image.h files to store the classification in .csv files.
6. Inject a single bit error in activation weights (first 1000 activation weights). This bash file automatize the process:
```textplain
#!/bin/bash
#from 0-31
BIT_FLIP=30
for idx_weight in `seq 0 1 1000`
do
    mkdir results/${idx_weight}_activationweight/
    ./bitflip yolov3.weights ${idx_weight} ${BIT_FLIP} yolov3_bitflip.weights
    # Execute YOLOv3 (10 000 iterations)
    ./darknet detector test ./cfg/coco.data ./cfg/yolov3.cfg ./yolov3_bitflip.weights -dont_show -ext_output <data/file_list.txt> results/results.csv
    cd results
    mv *.csv ${idx_weight}_activationweight/.
    cd ..
done
```

7. How to delete the last char of each line
``sed -i 's/.$//' file_name_2bemodified.txt ``
8. How to delete the fist characted of each line
``sed -i 's/^.//' file_name_2bemodified.txt ``
9. How to delete the first character of a string if its value is "0"
```plain 
// Delete first "0" character from the buffer
char aux_char[1] = "0";
if(strncmp(&buffer[0],&aux_char[0],1)==0)
{
	memmove(buffer, buffer+1, strlen(buffer));
	// printf("%c\n",buffer[0]);
} 
```


