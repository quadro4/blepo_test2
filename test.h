
#pragma once
#include <afxwin.h>  // necessary for MFC to work properly
#include "../../src/blepo.h"

using namespace blepo;
//using namespace blepo_ex;





int initial_filename_recognize( const CString path_input, const CString filename_input, int &value_low_treshold, int &value_high_treshold )
{
	CString initail_name1="fruit1.bmp";
	CString initail_name2="fruit2.bmp";
	CString initail_name3="fruit3.bmp";
	CString initail_name4="fruit4.bmp";
	CString initail_name5="fruit5.bmp";
	CString initail_name6="fruit6.bmp";

	if(   fopen(path_input+filename_input,"r")==0   )
	  { return 1; }
	
	
	
	if(  filename_input ==initail_name1 	)
	  { value_low_treshold=90; value_high_treshold=200;return 0; }



	else if( filename_input==initail_name2 )
			{value_low_treshold=80; value_high_treshold=200;return 0;}
	else if( 
			   ( filename_input ==initail_name3 )
			 ||( filename_input ==initail_name4 )
			 ||( filename_input ==initail_name5 )
		     ||( filename_input ==initail_name6 )
			 )  { value_low_treshold=75; value_high_treshold=166;return 0; }


	else  { return 2; }






	
}





/*
   Including cross seraching for reference points and then based on reference points to floodfill the region
   Remove dup reference points
  

   
*/

int img_floodfill(const ImgGray& img_in_lowtr, const ImgBinary& img_in_hightr, ImgBgr::Pixel new_color[], ImgBgr* img_out, int ref_point[][2], int* region_n )
{
	ImgBgr img_mid_1;
	ImgBgr img_mid_2;
	ImgBinary img_mid_3;
	int xx=1;
	//const int region_n=6;
	if( sizeof(img_in_lowtr(0,0))==1 )
	{
		
		img_out->Reset(img_in_lowtr.Width(), img_in_lowtr.Height());
		Convert(img_in_lowtr, img_out);

		img_mid_1.Reset(img_in_lowtr.Width(), img_in_lowtr.Height());
		Convert(img_in_lowtr, &img_mid_1);
		
		
	
	}
	else{	return 1;	}


	int ctn=0;

	for( int y=0; y< img_in_hightr.Height(); y++ ) 
	{
		for( int x=0; x< img_in_hightr.Width(); x++ ) 
		{
			
				int region_color_dup=0;			

				for( int ctn_1=0; ctn_1<*region_n; ctn_1++ )
				{
					if(  (*img_out)(x,y) == new_color[ctn_1]  ) 
					{ region_color_dup=1;break;}

					else
					{ region_color_dup=0; }								
				}//for ctn_1


				if( region_color_dup==0 && img_in_hightr(x,y)  && img_in_lowtr(x,y) )
				{
					ref_point[ctn][0]=x;
					ref_point[ctn][1]=y;
					FloodFill4( img_mid_1, x,  y, new_color[ctn], img_out);
					ctn++;
					printf("\n region number=%d \n", ctn);
					if( ctn==*region_n ) goto Func_end;

					
				}

				/*if( img_mid_2(x,y) == Bgr(0,0,0) )
				{
				ref_point[ctn][0]=x;
				ref_point[ctn][1]=y;
				FloodFill4( img_mid_1, x,  y, new_color[ctn], img_out);
				FloodFill4( img_mid_2, x,  y, Bgr(0,0,0), img_mid_2);
				}*/


			
		}// for y
	}// for x

Func_end:
	*region_n =ctn;// trans region_n(ctn) value to main
	
	
	
	/*Bgr* pix_filter;
    for (pix_filter = (*img_out).Begin() ; pix_filter != (*img_out).End() ; pix_filter++)
	{
		if(*pix_filter == Bgr(255,255,255) )  *pix_filter == Bgr(0,0,0);
	
	}*/
	
	printf("\n Seed points : \n");
	
	for (int ctn=0; ctn<*region_n ; ctn++) 
	{
		//if(ref_point[ctn][0]==0 && ref_point[ctn][1]==0)  continue;
		printf("Region: %d ,reference point:  X=%d  y=%d\n", ctn,ref_point[ctn][0],ref_point[ctn][1] );
	}
	
	for( int y=0; y< img_in_hightr.Height(); y++ ) 
	{
		for( int x=0; x< img_in_hightr.Width(); x++ ) 
		{
			if( (*img_out)(x,y)==Bgr(255,255,255) ) (*img_out)(x,y)==Bgr(0,0,0);
		}
	}



	return 0;


}//Func done




















/*
   Calculate regions' area and do filter to remove small regions.
  

   
*/
int region_area_compute(const ImgBinary img_in[], int region_area[], int region_center[][2], int region_offset[][2],const int region_n)
{
	
	//printf("%d  \n", region_n);

	for(int ctn=0; ctn<region_n;ctn++)
	{
		
		for(int y=0; y<(img_in)[ctn].Height(); y++)
		{
			for(int x=0; x<(img_in)[ctn].Width(); x++)
			{
				if(  (img_in)[ctn](x,y)==1 ) 	
				{	
					region_area[ctn]++;
					region_center[ctn][0]+=x;
					region_center[ctn][1]+=y;
				}

				//if( (*img_in)(x,y)==1) ) (*img_out)(x,y)=Bgr(0,0,0);	
			}//for x
			
		}//for y
	
		
		
		//printf("Region %d, area:  %d\n", ctn,region_area[ctn]);

		if( region_area[ctn]!=0 )
		{
			region_center[ctn][0] = region_center[ctn][0]/ region_area[ctn]+region_offset[ctn][0];
	  		region_center[ctn][1] = region_center[ctn][1]/ region_area[ctn]+region_offset[ctn][1];
		}
		else
		{ 	return 1;}
	}//for ctn
	

	return 0;
	
}























/*
   grasp regions to independent img sequence.
     
*/
void img_region_grasp(const ImgBgr& img_in, ImgBgr::Pixel region_colorspace[], ImgBinary img_out[], ImgBgr* img_out1,int region_offset[][2],const int region_n)
{
	//const int region_n=6; //for test
	ImgGray img_mid;
	ImgBinary img_erode_mid;
	img_mid.Reset(img_in.Width(), img_in.Height());
	Set(&img_mid, 0 );

	(*img_out1).Reset(img_in.Width(), img_in.Height());
	(*img_out1)=img_in;

	//initial region_rect
	const int offset_add=20;
	const int x_min=0, x_max=1, y_min=2, y_max=3;
	int region_rect[12][4]={0};

	for(int ctn=0; ctn<region_n; ctn++)
	{
		region_rect[ctn][x_min]=2000;
		region_rect[ctn][x_max]=0;

		region_rect[ctn][y_min]=2000;
		region_rect[ctn][y_max]=0;
	}


	//Scan pixel
	for(int ctn=0; ctn<region_n;ctn++)
	{
		
		for(int y=0; y<img_in.Height(); y++)
		{
			for(int x=0; x<img_in.Width(); x++)
			{
				//find target region
				//copy region
				if(  img_in(x,y)==region_colorspace[ctn] ) 	
				{

					
					img_mid(x,y)=255;
					
					//find region_rect
					//record min x
					if( x>0 &&img_in(x-1,y)==Bgr(0,0,0) )
					{
						if( x<region_rect[ctn][x_min] ) region_rect[ctn][x_min]=x;
					}
					else if(x==0) region_rect[ctn][x_min]=x;
					

					//record max x
					if( (x+1)<img_in.Width() &&img_in(x+1,y)==Bgr(0,0,0) )
					{
						if( x>region_rect[ctn][x_max] ) region_rect[ctn][x_max]=x;
					}
					else if( (x+1)==img_in.Width() ) region_rect[ctn][x_max]=x;

														
					//recoed min y
					if( y>0 &&img_in(x,y-1)==Bgr(0,0,0) )
					{
						if( y<region_rect[ctn][y_min] ) region_rect[ctn][y_min]=y;
					}
					else if( y==0 ) region_rect[ctn][y_min]=y;


					//record max y
					if( (y+1)<img_in.Height() &&img_in(x,y+1)==Bgr(0,0,0) )
					{
						if( y>region_rect[ctn][y_max] )  region_rect[ctn][y_max]=y;
					}
					else if( (y+1)==img_in.Height() ) region_rect[ctn][y_max]=y;
				}//if
				else if( img_in(x,y)==Bgr(255,255,255) )  (*img_out1)(x,y)=Bgr(0,0,0);

			}//for x
		}//for y

		//To avoid seeing a bug for output small photo. add offset value
		int dx=region_rect[ctn][x_max]-region_rect[ctn][x_min]+1;
		int	dy=region_rect[ctn][y_max]-region_rect[ctn][y_min]+1;
		
		region_offset[ctn][0] = region_rect[ctn][x_min]-offset_add;
		region_offset[ctn][1] = region_rect[ctn][y_min]-offset_add;


		(img_out)[ctn].Reset(dx+offset_add*2, dy+offset_add*2);
		Set( &img_out[ctn], 0 );

		for(int y=region_rect[ctn][y_min]-offset_add, yy=0 ; y<=region_rect[ctn][y_max]+offset_add; y++ , yy++)
		{
			for(int x=region_rect[ctn][x_min]-offset_add , xx=0 ; x<=region_rect[ctn][x_max]+offset_add; x++, xx++)
			{
				if( img_mid(x,y)==255 ) (img_out)[ctn](xx,yy) = 1;
				//if( img_mid(x,y)==0 ) (img_out)[ctn](xx,yy) = 0;
				
			}
		}
		Set( &img_mid, 0 );

		img_erode_mid.Reset( img_out[ctn].Width(), img_out[ctn].Height() );
		img_erode_mid=img_out[ctn];
		
		Dilate3x3(img_out[ctn], &img_erode_mid);
		Erode3x3(img_erode_mid, &img_out[ctn]);



	/*printf("\n");
	for(int ctn=0; ctn<region_n ; ctn++) 
	{
		printf("%d %d %d %d   \n", region_rect[x_min][ctn], region_rect[x_max][ctn], region_rect[y_min][ctn], region_rect[y_max][ctn]);
		printf("%d %d\n", dx,dy);
	}*/




	}// for ctn

}

/*
   Calculate perimeter with wall step
     
*/
int img_perimeter(const ImgBinary img_in[],  int region_perim[],ImgGray region_gray[], int const region_n)
{
	//const int region_n=6;
	ImgBinary img_mid1;
	ImgBinary img_mid2;

	for(int ctn=0;ctn<region_n;ctn++)
	{
		if(img_in[ctn].Width()==0 || img_in[ctn].Height()==0) return 1;
		
		
		img_mid1.Reset( img_in[ctn].Width(), img_in[ctn].Height() );
		//img_mid1[ctn]=img_in[ctn];
		Erode3x3(img_in[ctn],&img_mid1);

		img_mid2.Reset( img_in[ctn].Width(), img_in[ctn].Height() );
		Xor( img_in[ctn], img_mid1, &img_mid2);
		

		for(int y=0; y<img_in[ctn].Height(); y++)
		{
			for(int x=0; x<img_in[ctn].Width(); x++)
			{
				
				if(img_mid2(x,y) == 1) {region_perim[ctn]++;}
			}
		}


		region_gray[ctn].Reset(img_in[ctn].Width(), img_in[ctn].Height() );
		Convert(img_mid2, &region_gray[ctn]);

		
		/*ImgBinary::Iterator pix_val;
		for (pix_val = img_mid2.Begin() ; pix_val != img_mid2.End() ; pix_val++) 
		{
			if(*pix_val == 0) region_perim[ctn]++;
		}*/
		
		
		
	}
	return 0;
}








//int img_perimeter2(const ImgBinary img_in[], const int ref_point[][2],const int region_offset[][2], int region_perim[], ImgGray region_gray[], int const region_n)
//{
//
//
//
//
//	
//
//	/*img_mid2.Reset( img_in.Height(), img_in.Width() );
//	Convert(img_mid1, &img_mid2);*/
//	
//	
//	
//	int start_point[12][2]={0};
//	int list_perim[6]={0};
//
//	for(int ctn=0;ctn<region_n;ctn++)
//	{
//		
//		ImgGray img_mid;
//		img_mid.Reset( img_in[ctn].Height(), img_in[ctn].Width() );
//		Convert(img_in[ctn], &img_mid);
//		
//		
//		int loop1=1;
//		int loop2=1;
//		int xx=0,yy=0;
//
//		region_perim[ctn]=0;
//		xx=ref_point[ctn][0]-region_offset[ctn][0];
//		yy=ref_point[ctn][1]-region_offset[ctn][1];
//
//		//reach boundary
//
//
//		while (loop1)
//		{
//			if( img_in[ctn](xx,yy)==1 )
//			{
//				yy--;			
//			}
//			else
//			{
//				start_point[ctn][0]=xx;
//				start_point[ctn][1]=++yy;
//				break;
//				loop1=0;
//			}
//		}//while loop1
//
//		printf("\n%d  %d\n", xx,yy);
//
//		//begin to draw boundary
//		//(img_mid)(xx, yy)=127;
//		int dir = 0;  // points east
//		int move_f=0;
//		
//		while (loop2)
//		{
//			
//			
//			unsigned char left;
//		unsigned char front;
//
//			// grab pixel to left
//			
//			if (dir == 0)  // dir east 
//			{
//				if( yy==0 ) 
//				{left=0;}
//
//				else 
//				{left = (img_mid)(xx, yy-1);}//left pixel
//			}
//			else if (dir == 1) // dir north
//			{
//				if(xx==0) 
//				{left=0;}
//
//				else 
//				{left = (img_mid)(xx-1, yy);}//left
//			}
//			else if (dir == 2) // dir west
//			{
//				if( yy==img_in[ctn].Height()-1 ) 
//				{left=0;}
//
//				else 
//				{left = (img_mid)(xx, yy+1);}
//			}
//			else if (dir == 3) // dir south
//			{
//				if( xx==img_in[ctn].Width()-1 ) 
//				{left=0;}
//
//				else 
//				{left = (img_mid)(xx+1, yy);}
//			}
//
//			// grab pixel in front
//			
//			if (dir == 0)  // walking east
//			{
//				if( xx==img_in[ctn].Width()-1 ) 
//				{front=0;}
//				else 
//				{front =(img_mid)(xx+1, yy);}
//			}
//			else if (dir == 1) // walking north
//			{
//				if( yy==0 )
//				{front=0;}
//				else 
//				{front = (img_mid)(xx, yy-1);}
//			}
//			else if (dir == 2) // walking west
//			{
//				if(xx==0) 
//				{front=0;}
//				else 
//				{front = (img_mid)(xx-1, yy);}
//			}
//			else if (dir == 3) // walking south
//			{
//				if( yy==img_in[ctn].Height()-1 ) 
//				{front=0;}
//				else 
//				{front = (img_mid)(xx, yy+1);}
//			}
//
//
//
//			// now decide
//			if (left != 0) //left is on
//			{	dir++; move_f=1; }         	// turn left and move forward
//			else if (front == 0)
//			{	
//				dir--; 
//				if( dir==4 ) {dir=0;}
//				if( dir==-1) {dir=3;}
//				//continue; // turn right	
//			} 	
//			else
//			{	move_f=1; }// move forward
//
//
//
//
//
//			if( dir==4 ) {dir=0;}
//			if( dir==-1) {dir=3;}
//
//			if( move_f==1 )
//			{
//
//				if( dir==0 ) {xx++;}
//				else if( dir==1 ) {yy--;}
//				else if( dir==2 ) {xx--;}
//				else if( dir==3 ) {yy++;}
//				move_f=0;	
//
//				if( xx==start_point[ctn][0] && yy==start_point[ctn][1] )
//				{
//					(img_mid)(xx,yy)=127;
//					//(region_gray)[ctn](xx,yy) = 127;
//					break;
//					loop2=0;
//				}
//				else 
//				{	region_perim[ctn]++; 
//					(img_mid)(xx,yy)=127;
//					//(region_gray)[ctn](xx,yy) = 127;
//				}
//
//			}
//			/*printf("cycle\n");
//			printf("%d\n", img_mid(100,40));
//			printf("%d\n", img_mid(101,40));
//			printf("%d\n", img_mid(102,40));
//			printf("%d\n", img_mid(102,41));
//			printf("%d\n", img_mid(101,41));
//
//			printf("%d\n", img_mid(103,40));
//			printf("%d\n", img_mid(103,41));
//		
//			printf("%d\n", img_mid(100,39));
//			printf("%d\n", img_mid(101,39));
//			printf("%d\n", img_mid(102,39));
//			printf("%d\n", img_mid(103,39));*/
//		}//while loop2
//		(region_gray)[ctn]=img_mid;
//	}// for ctn
//	
//
//	return 0;
//}

/*
   Calculate moments from independent region img
     
*/
int moment_compute(const ImgBinary img_in, double moment_r[], double moment_c[])
{
	if( img_in.Width()==1 || img_in.Height()==1 ) {return 1;}
	
	
	//regualr monments
	int m00=0,m10=1, m01=2,m20=3, m02=4, m11=5;
	for(int y=0; y<img_in.Height(); y++)
	{
		for(int x=0; x<img_in.Width(); x++)
		{
			moment_r[m00]+=img_in(x,y);
			moment_r[m10]+=img_in(x,y) * x;
			moment_r[m01]+=img_in(x,y) * y;
			moment_r[m20]+=img_in(x,y) * x * x;
			moment_r[m02]+=img_in(x,y) * y * y;
			moment_r[m11]+=img_in(x,y) * x * y;
		
		}
	}
	//central monents
	int mu00=0, xbar=1, ybar=2,mu11=3,mu20=4,mu02=5;
	moment_c[mu00] = moment_r[m00];
	moment_c[xbar] = moment_r[m10] / moment_r[m00];
	moment_c[ybar] = moment_r[m01] / moment_r[m00];
	moment_c[mu11] = moment_r[m11] - moment_c[ybar] * moment_r[m10];
	moment_c[mu20] = moment_r[m20] - moment_c[xbar] * moment_r[m10];
	moment_c[mu02] = moment_r[m02] - moment_c[ybar] * moment_r[m01];
	return 0;
}














//void perimeter2(const ImgBinary& img_in,int ref_point[],int list_perim[])
//{
//	ImgBinary img_mid;
//	ImgBinary img_out;
//
//	img_out.Reset( img_in.Height(), img_in.Width() );
//	Set(&img_out, 0);
//
//	Erode3x3(img_in, &img_mid);
//
//	for(int y=0; y<img_in.Height(); y++)
//	{
//		for(int x=0; x<img_in.Width(); x++)
//		{
//			img_out(x,y) = ( img_in(x,y)!= img_mid(x,y) );
//
//		}
//
//	}
//
//}



/*
   Drawing outline of every region in oringal img.
     
*/

int img_outline(const ImgGray img_sub[], ImgBgr* img_io, ImgBgr::Pixel new_color[], int list_region_offset[][2], const int region_n)
{

	//const int region_n=6;
	for(int ctn=0; ctn<region_n ; ctn++)
	{
		



		for(int y=0; y<img_sub[ctn].Height(); y++)
		{
			for(int x=0; x<img_sub[ctn].Width(); x++)
			{
				if(   x+list_region_offset[ctn][0] >=(*img_io).Width() 
					&&y+list_region_offset[ctn][1] >=(*img_io).Height()	)
				{
					return 1;
				}
				
				if( img_sub[ctn](x,y) == 255 )
				{
					int xx= x+list_region_offset[ctn][0];
					int yy= y+list_region_offset[ctn][1];
					
					
					//printf("%d , %d\n",x,y);
					(*img_io)(xx,yy)=new_color[ctn];
					
				}
				
				
	
	
	
			}//for y
		}//for x
	
	}//for ctn
	return 0;
}



