#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#define PATH "t.pcm"
using namespace std;

//1帧的大小=每秒播放的音频字节大小=声道数*采样个数(采样频率是1秒采样的个数,1帧固定为1024)*采样位数(Hz)  一帧的播放时长/一帧(1024) = 1000ms/44100     1帧播放时间(ms) = 样本数*1000/采样率

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
    fp1.open("output_doublespeed.pcm",ios::out|ios::binary|ios::trunc);

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

int pcm16le_to_pcm8(char *path){
    ifstream fp;
    fp.open(path,ios::in|ios::binary);
    ofstream fp1;
    fp1.open("output_8.pcm",ios::out|ios::binary|ios::trunc);
    unsigned char *sample = (unsigned char*)malloc(4);

    /*16的取值：-2^15~2^15-1  8的取值0~255，先将-32768~32767转换为-128~127，再转为0~255*/
    /*注意，在AE中选格式 选pcm8的无符号格式*/
    while(!fp.eof()){
        short* samplenum16 = NULL;
        char samplenum8 = 0;
        unsigned char samplenum8_u = 0;
        fp.read((char*)sample,4);
        samplenum16 = (short*)sample;
        samplenum8=(*samplenum16)>>8;  //>> 二进制右移运算符 将一个数的各二进制位全部右移若干（这里是8）位，正数左补0，负数左补1，右边丢弃
        samplenum8_u=samplenum8+128;
        fp1.write((const char*)&samplenum8_u,1);

        samplenum16 = (short*)(sample+2);
        samplenum8=(*samplenum16)>>8;  
        samplenum8_u=samplenum8+128;
        fp1.write((const char*)&samplenum8_u,1);
    }

    free(sample);
    fp.close();
    fp1.close();
    return 0;
}

//将MP3处理为pcm16le  ffmpeg -i t.singlechannel.mp3 -f s16le -acodec pcm_s16le -ac 1 -ar 44100 t_singlechannel.pcm
//转双声道为单声道  ffmpeg -i t.mp3 -ac 1 t.singlechannel.mp3
int pcm16le_cut_singlechannel(char *path,int start_time,int dur_time){ //单声道的pcm16le 
    ifstream fp;
    fp.open(path,ios::in|ios::binary);
    ofstream fp1,fp2;
    fp1.open("output_cut.pcm",ios::out|ios::binary|ios::trunc);
    fp2.open("output_cut_stat.txt",ios::out|ios::trunc);
    unsigned char *sample = (unsigned char*)malloc(2);
    int cnt = 0;
    while(!fp.eof()){
        fp.read((char*)sample,2);
        if(cnt>start_time&&cnt<=(start_time+dur_time)){
            fp1.write((const char*)sample,2);
            short samplenum = samplenum*256;
            samplenum = samplenum+sample[0];
            fp2<<samplenum<<",";
            if(cnt%10==0){
                fp2<<samplenum<<endl;
            }
        }
        cnt++;
    }
    free(sample);
    fp.close();
    fp1.close();
    fp2.close();
    return 0;
}

int pcm16le_cut(char *path,int start_time,int dur_time){ //双声道pcm16le
    ifstream fp;
    fp.open(path,ios::in|ios::binary);
    ofstream fp1,fp2;
    fp1.open("output_cut_doublechannel.pcm",ios::out|ios::binary|ios::trunc);
    //fp2.open("output_cut_stat.txt",ios::out|ios::trunc);
    unsigned char *sample = (unsigned char*)malloc(4);
    int cnt = 0;
    while(!fp.eof()){
        fp.read((char*)sample,4);
        if(cnt>start_time&&cnt<=(start_time+dur_time)){
            fp1.write((const char*)sample,2);
            fp1.write((const char*)sample,2);
            //short samplenum = samplenum*256;
            //samplenum = samplenum+sample[0];
            /*
            fp2<<samplenum<<",";
            if(cnt%10==0){
                fp2<<samplenum<<endl;
            }
            */
        }
        cnt++;
    }
    free(sample);
    fp.close();
    fp1.close();
    fp2.close();
    return 0;
}

/*wav文件的实质就是在pcm文件前面加文件头  需要了解riff格式文件 

采样由四大模块组成
WAVE_HEADER

WAVE_FMT

WAVE_DATA

PCM数据 */
int pcm16le_to_wav(const char *pcmpath,int channels,int sample_rate,const char *wavpath){


    class WAVE_HEADER{
        public:
            char fccID[4];
            unsigned long dwSize;  //文件长度信息   必须写入音频采样数据后才能获得 故WAVE_HEADER 和 WAVE_DATA最后写入
            char fccType[4];
    };

    class WAVE_FMT{
        public:
            char fccID[4];
            unsigned long dwSize;  //文件长度信息
            unsigned short wFormatTag;    
            unsigned short wChannels;  
            unsigned long  dwSamplesPerSec;  
            unsigned long  dwAvgBytesPerSec;  
            unsigned short wBlockAlign;  
            unsigned short uiBitsPerSample;  
    };

    class WAVE_DATA{
        public:
            char fccID[4];
            unsigned long dwSize; //文件长度信息  必须写入音频采样数据后才能获得
    };

    if(channels==0||sample_rate==0){
        channels = 2;
        sample_rate = 44100;
    }

    int bits = 16;

    WAVE_HEADER pcmHEADER;
    WAVE_FMT pcmFMT;
    WAVE_DATA pcmDATA;

    unsigned short m_pcmData;
    ifstream fp;
    ofstream fpout;
    fp.open(pcmpath,ios::in|ios::binary);
    fpout.open(wavpath,ios::out|ios::binary|ios::trunc);

    if(!fp.is_open()){
        cout<<"打开pcm文件失败"<<endl;
        return -1;
    }

    if(!fpout.is_open()){
        cout<<"创建wav文件失败"<<endl;
        return -1;
    }

    memcpy(pcmHEADER.fccID,"RIFF",strlen("RIFF")); // void *memcpy(void *str1, const void *str2, size_t n) 从存储区 str2 复制 n 个字节到存储区 str1
    memcpy(pcmHEADER.fccType,"WAVE",strlen("WAVE"));
    fpout.seekp(sizeof(WAVE_HEADER),ios::cur); //定位文件位置指针，第一个参数：定位到指定大小的字节，第二个参数指定查找方向
    
    pcmFMT.dwSamplesPerSec = sample_rate;
    pcmFMT.dwAvgBytesPerSec = pcmFMT.dwSamplesPerSec*sizeof(m_pcmData);
    pcmFMT.uiBitsPerSample = bits;
    memcpy(pcmFMT.fccID,"fmt ",strlen("fmt "));
    pcmFMT.dwSize = 16;
    pcmFMT.wBlockAlign = 2;    
    pcmFMT.wChannels = channels;
    pcmFMT.wFormatTag = 1;

    fpout.write((const char*)&pcmFMT,sizeof(WAVE_FMT));


    memcpy(pcmDATA.fccID,"data",strlen("data"));
    pcmDATA.dwSize = 0;
    fpout.seekp(sizeof(WAVE_DATA),ios::cur);

    fp.read((char *)&m_pcmData,sizeof(unsigned short));
    while(!fp.eof()){
        pcmDATA.dwSize +=2;
        fpout.write((const char*)&m_pcmData,sizeof(unsigned short));
        fp.read((char *)&m_pcmData,sizeof(unsigned short));
    }

    pcmHEADER.dwSize = 44 + pcmDATA.dwSize;
    
    fpout.clear(ios::goodbit);//rewind()
    fpout.seekp(ios::beg);
    fpout.write((const char*)&pcmHEADER,sizeof(WAVE_HEADER));
    fpout.seekp(sizeof(WAVE_FMT),ios::cur);
    fpout.write((const char*)&pcmDATA,sizeof(WAVE_DATA));
    
    fp.close();
    fpout.close();
    
    return 0;
}



int main(){
    //pcm16le_split("t.pcm");
    //pcm16le_halfvolume_lest("t.pcm");
    //pcm16le_doublespeed(PATH);
    //pcm16le_to_pcm8(PATH);
   // pcm16le_cut_singlechannel("t_singlechannel.pcm",2360,120);  //长度为0.5s的音频包含22050个采样点,从第2360点开始，到120点   
    //pcm16le_cut_singlechannel("t_singlechannel.pcm",2,88200); 
    //pcm16le_to_wav("t.pcm",2,44100,"output.wav");
    pcm16le_cut(PATH,0,88200);
    //system("pause");
    return 0;
}