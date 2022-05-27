
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdbool.h>
#include<string.h>



/*
@param num 帧数

*/

int yuv420_split(char *path,int w,int h,int num) {
	FILE *fp = fopen(path, "rb+");
	FILE *fp_y = fopen("output1_420_y.y", "wb+");
	FILE *fp_u = fopen("output1_420_u.y", "wb+");
	FILE *fp_v = fopen("output1_420_v.y", "wb+");

	unsigned char *pic = (unsigned char*)malloc(w*h * 3 / 2);
	for (int i = 0; i < num; i++) {
		fread(pic, 1, w*h * 3 / 2, fp);
		fwrite(pic, 1, w*h , fp_y);
		fwrite(pic + w * h, 1, w*h / 4, fp_u);
		fwrite(pic + w * h * 5 / 4, 1, w*h / 4, fp_v);
	}
	free(pic);
	fclose(fp);
	fclose(fp_y);
	fclose(fp_u);
	fclose(fp_v);
	return 0;

}

int yuv444_split(char *path,int w,int h,int num) {
	FILE *fp = fopen(path, "rb+");
	FILE *fp_y = fopen("output1_444_y.y", "wb+");
	FILE *fp_u = fopen("output1_444_u.y", "wb+");
	FILE *fp_v = fopen("output1_444_v.y", "wb+");

	unsigned char *pic = (unsigned char*)malloc(w*h * 3 / 2);
	for (int i = 0; i < num; i++) {
		fread(pic, 1, w*h * 3, fp);
		fwrite(pic, 1, w*h , fp_y);
		fwrite(pic + w * h, 1, w*h, fp_u);
		fwrite(pic + w * h * 2, 1, w*h, fp_v);
	}
	free(pic);
	fclose(fp);
	fclose(fp_y);
	fclose(fp_u);
	fclose(fp_v);
	return 0;

}

int yuv420_bright_half(char *path,int w,int h,int num){
	FILE *fp=fopen(path,"rb+");
	FILE *fp1=fopen("output_half.yuv","wb+");

	unsigned char *pic=(unsigned char*)malloc(w*h*3/2);

	for(int i=0;i<num;i++){
		fread(pic,1,w*h*3/2,fp);
		for(int j=0;j<w*h;j++){
			unsigned char temp = pic[j]/2; //亮度减半
			pic[j]=temp;
		}
		fwrite(pic,w*h*3/2,1,fp1);
	}
	free(pic);
	fclose(fp);
	fclose(fp1);

	return 0;
}

int yuv420_border(char *path,int w,int h,int border,int num){
	FILE *fp = fopen(path,"rb+");
	FILE *fp1 = fopen("output_border.yuv","wb+");
	unsigned char *pic = (unsigned char *)malloc(w*h*3/2);
	for(int i=0;i<num;i++){
		fread(pic,1,w*h*3/2,fp);
		for(int j=0;j<h;j++){
			for(int k=0;k<w;k++){
				if(k<border||k>(w-border)||j<border||j>(h-border)){
					pic[j*w+k]=255;
					;
				}
			}
		}
		fwrite(pic,1,w*h*3/2,fp1);
	}

	free(pic);
	fclose(fp);
	fclose(fp1);

	return 0;
}

/*
param:
y_min 灰度值的最小值 例 0
y_max 灰度值的最大值 例 255
*/

int yuv420_graybar(int w,int h,int y_min,int y_max,int barnum,char *output_path){ 
	int barwidth;
	float lum_inc;
	unsigned char lum_temp;
	int uv_w,uv_h;
	FILE *fp = NULL;
	unsigned char *fp_y=NULL;
	unsigned char *fp_u=NULL;
	unsigned char *fp_v=NULL;
	
	barwidth =w/barnum; //每一个灰度块的宽度
	lum_inc=((float)(y_max-y_min)/((float)(barnum-1))); //灰度值增量 lum：luma luminance
	uv_w=w/2;
	uv_h=h/2;

	fp_y=(unsigned char *)malloc(w*h);
	fp_u=(unsigned char *)malloc(uv_h*uv_w);
	fp_v=(unsigned char *)malloc(uv_h*uv_w);

	if((fp=fopen(output_path,"wb+"))==NULL){
		printf("failed!");
		return -1;
	}


	for(int t=0;t<(w/barwidth);t++){
		lum_temp=y_min+(char)(t*lum_inc);
		printf("lum_inc:%3d",lum_inc);
		printf("lum_temp:%3d\n",lum_temp);
	}

	for(int j=0;j<h;j++){
		for(int i=0;i<w;i++){
			int t=i/barwidth;
			lum_temp=y_min+(char)(t*lum_inc);
			fp_y[j*w+i]=lum_temp;
		}
	}

	for(int j=0;j<uv_h;j++){
		for(int i=0;i<uv_w;i++){
			fp_u[j*uv_w+i]=128;
		}
	}
		for(int j=0;j<uv_h;j++){
			for(int i=0;i<uv_w;i++){
				fp_v[j*uv_w+i]=128;
			}
		}

	fwrite(fp_y,w*h,1,fp);
	fwrite(fp_u,uv_w*uv_h,1,fp);
	fwrite(fp_v,uv_w*uv_h,1,fp);
	fclose(fp);
	free(fp_y);
	free(fp_u);
	free(fp_v);
	return 0;
}

int yuv420_psnr(char *path1,char *path2,int w,int h,int num){
	FILE *fp1=fopen(path1,"rb+");
	FILE *fp2=fopen(path2,"rb+");
	unsigned char *pic1=(unsigned char *)malloc(w*h);
	unsigned char *pic2=(unsigned char *)malloc(w*h);
 
	for(int i=0;i<num;i++){
		fread(pic1,1,w*h,fp1);
		fread(pic2,1,w*h,fp2);
 
		double mse_sum=0,mse=0,psnr=0;
		for(int j=0;j<w*h;j++){
			mse_sum+=pow((double)(pic1[j]-pic2[j]),2);  //平方
		}
		mse=mse_sum/(w*h);
		psnr=10*log10(255.0*255.0/mse); //取对数
		printf("%5.3f\n",psnr);
 
		fseek(fp1,w*h/2,SEEK_CUR); //移动文件流的读写位置 SEEK_CUR 文件指针当前位置
		fseek(fp2,w*h/2,SEEK_CUR);
 
	}
 
	free(pic1);
	free(pic2);
	fclose(fp1);
	fclose(fp2);
	system("pause");
	return 0;
}


/*将rgb24转bmp，先不研究*/
int simplest_rgb24_to_bmp(const char *rgb24path,int width,int height,const char *bmppath){
	typedef struct 
	{  
		long imageSize;
		long blank;
		long startPosition;
	}BmpHead;
 
	typedef struct
	{
		long  Length;
		long  width;
		long  height;
		unsigned short  colorPlane;
		unsigned short  bitColor;
		long  zipFormat;
		long  realSize;
		long  xPels;
		long  yPels;
		long  colorUse;
		long  colorImportant;
	}InfoHead;
 
	int i=0,j=0;
	BmpHead m_BMPHeader={0};
	InfoHead  m_BMPInfoHeader={0};
	char bfType[2]={'B','M'};
	int header_size=sizeof(bfType)+sizeof(BmpHead)+sizeof(InfoHead);
	unsigned char *rgb24_buffer=NULL;
	FILE *fp_rgb24=NULL,*fp_bmp=NULL;
 
	if((fp_rgb24=fopen(rgb24path,"rb"))==NULL){
		printf("Error: Cannot open input RGB24 file.\n");
		return -1;
	}
	if((fp_bmp=fopen(bmppath,"wb"))==NULL){
		printf("Error: Cannot open output BMP file.\n");
		return -1;
	}
 
	rgb24_buffer=(unsigned char *)malloc(width*height*3);
	fread(rgb24_buffer,1,width*height*3,fp_rgb24);
 
	m_BMPHeader.imageSize=3*width*height+header_size;
	m_BMPHeader.startPosition=header_size;
 
	m_BMPInfoHeader.Length=sizeof(InfoHead); 
	m_BMPInfoHeader.width=width;
	//BMP storage pixel data in opposite direction of Y-axis (from bottom to top).
	m_BMPInfoHeader.height=-height;
	m_BMPInfoHeader.colorPlane=1;
	m_BMPInfoHeader.bitColor=24;
	m_BMPInfoHeader.realSize=3*width*height;
 
	fwrite(bfType,1,sizeof(bfType),fp_bmp);
	fwrite(&m_BMPHeader,1,sizeof(m_BMPHeader),fp_bmp);
	fwrite(&m_BMPInfoHeader,1,sizeof(m_BMPInfoHeader),fp_bmp);
 
	//BMP save R1|G1|B1,R2|G2|B2 as B1|G1|R1,B2|G2|R2
	//It saves pixel data in Little Endian
	//So we change 'R' and 'B'
	for(j =0;j<height;j++){
		for(i=0;i<width;i++){
			char temp=rgb24_buffer[(j*width+i)*3+2];
			rgb24_buffer[(j*width+i)*3+2]=rgb24_buffer[(j*width+i)*3+0];
			rgb24_buffer[(j*width+i)*3+0]=temp;
		}
	}
	fwrite(rgb24_buffer,3*width*height,1,fp_bmp);
	fclose(fp_rgb24);
	fclose(fp_bmp);
	free(rgb24_buffer);
	printf("Finish generate %s!\n",bmppath);
	return 0;
}

/*
用ffmpeg 转图片为rgb24格式 ffmpeg -i cie.jpg -pix_fmt rgb24 cie.yuv
在yuvplayer查看y分量，大小和原来图片大小相同
*/
int rgb24_split(char *path, int w, int h,int num){
	FILE *fp=fopen(path,"rb+");
	FILE *fp_r=fopen("output_r.y","wb+");
	FILE *fp_g=fopen("output_g.y","wb+");
	FILE *fp_b=fopen("output_b.y","wb+");
 
	unsigned char *pic=(unsigned char *)malloc(w*h*3);
 
	for(int i=0;i<num;i++){
 
		fread(pic,1,w*h*3,fp);
 
		for(int j=0;j<w*h*3;j=j+3){
			//R
			fwrite(pic+j,1,1,fp_r);
			//G
			fwrite(pic+j+1,1,1,fp_g);
			//B
			fwrite(pic+j+2,1,1,fp_b);
		}
	}
 
	free(pic);
	fclose(fp);
	fclose(fp_r);
	fclose(fp_g);
	fclose(fp_b);
 
	return 0;
}



unsigned char clip_value(unsigned char x,unsigned char min_val,unsigned char  max_val){
	if(x>max_val){
		return max_val;
	}else if(x<min_val){
		return min_val;
	}else{
		return x;
	}
}
 
//RGB to YUV420
bool RGB24_TO_YUV420(unsigned char *RgbBuf,int w,int h,unsigned char *yuvBuf)
{
	unsigned char*ptrY, *ptrU, *ptrV, *ptrRGB;
	memset(yuvBuf,0,w*h*3/2); //将0复制到yuvBuf指向的字符串的前w*h*3/2个字符
	ptrY = yuvBuf;
	ptrU = yuvBuf + w*h;
	ptrV = ptrU + (w*h*1/4);
	unsigned char y, u, v, r, g, b;
	for (int j = 0; j<h;j++){
		ptrRGB = RgbBuf + w*j*3 ;
		for (int i = 0;i<w;i++){
			
			r = *(ptrRGB++);
			g = *(ptrRGB++);
			b = *(ptrRGB++);
			y = (unsigned char)( ( 66 * r + 129 * g +  25 * b + 128) >> 8) + 16  ;          
			u = (unsigned char)( ( -38 * r -  74 * g + 112 * b + 128) >> 8) + 128 ;          
			v = (unsigned char)( ( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128 ;
			*(ptrY++) = clip_value(y,0,255);
			if (j%2==0&&i%2 ==0){
				*(ptrU++) =clip_value(u,0,255);
			}
			else{
				if (i%2==0){
				*(ptrV++) =clip_value(v,0,255);
				}
			}
		}
	}
	return true;
}
 
/**
 * Convert RGB24 file to YUV420P file
 * @param url_in  Location of Input RGB file.
 * @param w       Width of Input RGB file.
 * @param h       Height of Input RGB file.
 * @param num     Number of frames to process.
 * @param url_out Location of Output YUV file.
 */
int simplest_rgb24_to_yuv420(char *url_in, int w, int h,int num,char *url_out){
	FILE *fp=fopen(url_in,"rb+");
	FILE *fp1=fopen(url_out,"wb+");
 
	unsigned char *pic_rgb24=(unsigned char *)malloc(w*h*3);
	unsigned char *pic_yuv420=(unsigned char *)malloc(w*h*3/2);
 
	for(int i=0;i<num;i++){
		fread(pic_rgb24,1,w*h*3,fp);
		RGB24_TO_YUV420(pic_rgb24,w,h,pic_yuv420);
		fwrite(pic_yuv420,1,w*h*3/2,fp1);
	}
 
	free(pic_rgb24);
	free(pic_yuv420);
	fclose(fp);
	fclose(fp1);
 
	return 0;
}

int rgb24_colorbar(int width, int height,char *url_out){
 
	unsigned char *data=NULL;
	int barwidth;
	char filename[100]={0};
	FILE *fp=NULL;
	int i=0,j=0;
 
	data=(unsigned char *)malloc(width*height*3);
	barwidth=width/8;
 
	if((fp=fopen(url_out,"wb+"))==NULL){
		printf("Error: Cannot create file!");
		return -1;
	}
 
	for(j=0;j<height;j++){
		for(i=0;i<width;i++){
			int barnum=i/barwidth;
			switch(barnum){
			case 0:{
				data[(j*width+i)*3+0]=255;
				data[(j*width+i)*3+1]=255;
				data[(j*width+i)*3+2]=255;
				break;
				   }
			case 1:{
				data[(j*width+i)*3+0]=255;
				data[(j*width+i)*3+1]=255;
				data[(j*width+i)*3+2]=0;
				break;
				   }
			case 2:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=255;
				data[(j*width+i)*3+2]=255;
				break;
				   }
			case 3:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=255;
				data[(j*width+i)*3+2]=0;
				break;
				   }
			case 4:{
				data[(j*width+i)*3+0]=255;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=255;
				break;
				   }
			case 5:{
				data[(j*width+i)*3+0]=255;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=0;
				break;
				   }
			case 6:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=255;
 
				break;
				   }
			case 7:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=0;
				break;
				   }
			}
 
		}
	}
	fwrite(data,width*height*3,1,fp);
	fclose(fp);
	free(data);
 
	return 0;
}

int main()
{

	//yuv420_split("test.yuv",1920,1080,1);
	//yuv420_bright_half("test.yuv",1920,1080,1);
	//yuv420_border("test.yuv",1920,1080,100,1);
	//yuv420_graybar(640,360,0,255,10,"graybar.yuv");
	//yuv420_psnr("cie.yuv","cie2.yuv",489,520,1);
	//rgb24_split("cie.yuv",1440,1530,1);
	rgb24_colorbar(640,360,"colorbar.rgb");
	return 0;
}

/*
int simplest_yuv420_split(char *url, int w, int h,int num){
	FILE *fp=fopen(url,"rb+");
	FILE *fp1=fopen("output_420_y.y","wb+");
	FILE *fp2=fopen("output_420_u.y","wb+");
	FILE *fp3=fopen("output_420_v.y","wb+");
 
	unsigned char *pic=(unsigned char *)malloc(w*h*3/2);
 
	for(int i=0;i<num;i++){
 
		fread(pic,1,w*h*3/2,fp);
		//Y
		fwrite(pic,1,w*h,fp1);
		//U
		fwrite(pic+w*h,1,w*h/4,fp2);
		//V
		fwrite(pic+w*h*5/4,1,w*h/4,fp3);
	}
 
	free(pic);
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
 
	return 0;
}

int main()
{
	simplest_yuv420_split("1.yuv",256,256,1);
	return 0;
}
*/