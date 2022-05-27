#include <iostream>
#include <stdlib.h>
#include <fstream>
using namespace std;
int pcm16le_split(char *path){
    ifstream fp;
    fp.open(path,ios::in|ios::binary);
    ofstream fp_l,fp_r;
    fp_l.open("output_l.pcm",ios::out | ios::trunc |ios::binary);
    fp_r.open("output_r.pcm",ios::out | ios::trunc |ios::binary);
    unsigned char *sample=(unsigned char*)malloc(4);//每个声道的每个采样占用2byte，pcm16le双声道数据的左声道和右声道是间隔储存的
    while(!fp.eof()){
        fp.read((char*)sample,4);
        fp_l.write((const char*)sample,2);
        fp_r.write((const char*)sample+2,2);
    }
    free(sample);
    fp.close();
    fp_l.close();
    fp_r.close();
    return 0;
}

int pcm16le_halfvolume_lest(char *path){
    ifstream fp;
    fp.open(path,ios::in|ios::binary);
    ofstream fp1;
    fp1.open("output_halfvolume_left.pcm",ios::out|ios::trunc|ios::binary);
    
    unsigned char *sample=(unsigned char*)malloc(4);

    while(!fp.eof()){
        short *samplenum=NULL;
        fp.read((char*)sample,4);
        samplenum=(short *)sample;  //samplenum是指向sample的指针，通过修改samplenum来修改sample，而sample+2没有被修改
        *samplenum=*samplenum/2;  
        fp1.write((const char*)sample,2);
        fp1.write((const char*)sample+2,2); 

    }

    free(sample);
    fp.close();
    fp1.close();
    return 0;
}

int pcm16le_doublespeed(char *path){

    ifstream fp;
    fp.open(path,ios::in|ios::binary);

    ofstream fp1;
    fp.open("output_doublespeed.pcm",ios::out|ios::binary|ios::trunc);

    int cnt = 0;
    unsigned char *sample = (unsigned char*)malloc(4);

    while(!fp.eof()){
        fp.read((char*)sample,4);
        if(cnt%2!=0){
            fp1.write((const char*)sample,2);//L
            fp1.write((const char*)sample+2,2);//R
            //采样数倍减
        }
        cnt=cnt+1;
    }

    free(sample);
    fp.close();
    fp1.close();
    return 0;
}

int main(){
    //pcm16le_split("t.pcm");
    //pcm16le_halfvolume_lest("t.pcm");
    pcm16le_doublespeed("t.pcm");
    //system("pause");
    return 0;
}