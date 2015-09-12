


#include <afxwin.h>  // necessary for MFC to work properly
#include "test.h"
#include "../../src/blepo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif





using namespace blepo;
//using namespace blepo_ex;


int main(int argc, const char* argv[], const char* envp[])
{
	
	if( argc > 1)	
	{
		printf("Error: more command parameters than use \n");
	}
	else
	{
		printf("argc = %d\n", argc);
		for (int i=0; i<argc ; i++) 
		{
			printf("argv[%d]=%s\n\n", i, argv[i]);
		}
	}

	HMODULE hModule = ::GetModuleHandle(NULL);
	if (hModule == NULL || !AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
	{
		printf("Fatal Error: MFC initialization failed (hModule = %x)\n", hModule);
		return 1;
	}
	
	
	
	
	printf("  \n\n");
	printf("a. \Start: n");

	/*
	a.	Reads 1 command-line parameter, which we will call filename.  
	b.	Loads filename from the blepo/images directory into a Grayscale or BGR image and displays it in a figure window.
	*/
	CString path_blepo_images ="../../images/";
	CString filename_input=argv[1];
	CString filename_forloading = path_blepo_images + argv[1];
	
	
	
	//Judge File name that is whether incomplete before loading
	//Initial Low/High treshold value
	int low_tr_val=90;
	int high_tr_val=180;

	int status_filename=initial_filename_recognize(path_blepo_images, filename_input, low_tr_val,high_tr_val);

	if ( status_filename ==0 )
	{
		printf("Act: Get initial threshold value for: %s \n",filename_forloading );
	
	}
	else if( status_filename==2 )
	{
		printf("Warning: Cannot find initial threshold value for: %s \n",filename_forloading );
	}
	else if( status_filename==1 )
	{
		printf("Error: File cannot be find : %s \n",filename_forloading );	
		printf("Act: Program halt, Please close \n");
		EventLoop();
		return 0;
	}




	//Val initial 
	ImgGray img_loaded_image_g;
	//ImgGray img1_swp;
	ImgBinary img_low_threshold_b;//_b=binary
	ImgBinary img_high_threshold_b;//_b=binary 
	ImgGray img_low_threshold_g;
	ImgGray img_tr_tmp;
	ImgBgr img_region_diff;
	ImgBgr img_region_diff2;
	

	//Figure and title
	Figure fig_loaded_image, fig_high_threshold, fig_low_threshold,fig_region_diff, fig_out_f;
	
	fig_loaded_image.SetTitle("Loaded File");
	fig_high_threshold.SetTitle("High Threshold");
	fig_low_threshold.SetTitle("Low Threshold with noise removal");
	fig_out_f.SetTitle("Final out fig");
	fig_region_diff.SetTitle("Double Threshold");


	// Loading filename and show it 
	Load(filename_forloading, &img_loaded_image_g);
	fig_loaded_image.Draw(img_loaded_image_g);



	/*c.	Performs double thresholding using two thresholds that you determine by trial and error, which are hardcoded in your code.*/

	Threshold(img_loaded_image_g, low_tr_val, &img_low_threshold_b);//Low Thresholding
	Threshold(img_loaded_image_g, high_tr_val, &img_high_threshold_b);//High Thresholding

	fig_high_threshold.Draw(img_high_threshold_b);
	

	
	/*d.	Perform noise removal (if needed) using mathematical morphology (i.e., some combination of erosion / dilation / opening / closing) either before or during thresholding.*/
	Convert(img_low_threshold_b, &img_low_threshold_g);
	Erode3x3    (img_low_threshold_g, &img_tr_tmp);
	Erode3x3      (img_tr_tmp, &img_low_threshold_g);

	Dilate3x3    (img_low_threshold_g, &img_tr_tmp);
	Dilate3x3    (img_tr_tmp, &img_low_threshold_g);

	fig_low_threshold.Draw(img_low_threshold_g);


	/*e.	Runs connected components (either the classic union-find algorithm or by repeated applications of floodfill) to detect and count the foreground regions 
		in the cleaned, thresholded image, distinguishing them from the background.  */
	printf("  \n\n");
	printf("  Color space\n");   
	const int region_n_defa=12;
	int region_n=12;

	int ref_point[region_n_defa][2] = {0};
	ImgBinary region_binary[region_n_defa];
	ImgGray region_gray[region_n_defa];
	
	ImgBgr::Pixel new_color[region_n_defa]={Bgr(0,0,0)};
	int color_1=(255-1)/region_n_defa;
	int color_2=0;



	for (int ctn=0; ctn<region_n_defa; ctn++) 
	{
		if(ctn<region_n_defa/2)  
		{
			new_color[ctn] = Bgr(color_2,0,127); 
			color_2=color_2+color_1+color_1;
		}

		if(ctn==region_n_defa/2) 
		{
			new_color[ctn] = Bgr(255,255,0);
			color_2=0;
		}


		if(ctn>region_n_defa/2)
		{
			new_color[ctn] = Bgr(0,color_2,255);
			color_2=color_2+color_1+color_1;
		}
		printf("%d  BGR= (%d, %d, %d)\n",ctn ,new_color[ctn].b,new_color[ctn].g,new_color[ctn].r);
	}



	
	
	if( img_floodfill(img_low_threshold_g, img_high_threshold_b, new_color, &img_region_diff,ref_point,&region_n )!=0 )  {printf("Error: Floodfill or Find Seed points Undone \n");}
	
		
	
	


	//f.	Computes the properties of each foreground region, including
	printf("  \n\n");
	printf("b. Properties\n");
	//	ii.	compactness (To compute the area, simply count the number of pixels.  
	//		To compute the perimeter, apply the logical XOR to the thresholded image and the result of eroding this image with a 3x3 structuring element of all ones; 
	//		the result will be the number of 4-connected foreground boundary pixels.)
	
	//area
	int properties_area[region_n_defa]={0}; 
	int properties_perimeter[region_n_defa]={0};
	int properties_center_point[region_n_defa][2]={0};
	double properties_compactness[region_n_defa]={0};
	int properties_region_offset[region_n_defa][2]={0};
	
	
	
	

	img_region_grasp(img_region_diff, new_color, region_binary, &img_region_diff2,properties_region_offset,region_n);


	fig_region_diff.Draw(img_region_diff2);
	
	if( region_area_compute( region_binary, properties_area, properties_center_point, properties_region_offset ,region_n) != 0 ) {printf("Error: area calculation problem\n\n");}
	
	
	
	

	


	
	for(int ctn=0; ctn<region_n ; ctn++) 
	{
		region_gray[ctn].Reset(region_binary[ctn].Width(), region_binary[ctn].Height());
		Convert(region_binary[ctn], &region_gray[ctn]);
		
	}



	

	if( img_perimeter(region_binary, properties_perimeter, region_gray, region_n)==0 )  {printf("perimeter done\n\n");}
	//if( img_perimeter2(region_binary, ref_point, list_region_offset, list_perim, region_gray, region_n) ==0 ) {printf("perimeter done\n\n");}
	
	/*for(int ctn=0;ctn<region_n;ctn++)
	{
		list_perim[ctn]=0;
		for(int y=0; y<region_gray[ctn].Height(); y++)
		{
			for(int x=0; x<region_gray[ctn].Width(); x++)
			{
				
				if(region_gray[ctn](x,y) == 255) {region_gray[ctn](x,y)=127; list_perim[ctn]++;}
			}
		}

	}*/
	

	
	//printf("Compactness:  \n");
	for (int ctn=0; ctn<region_n ; ctn++) 
	{
		printf("\n");
		printf("  Region %d: \n", ctn);
		Figure fig_se;
		properties_compactness[ctn]= ( 4*3.14*properties_area[ctn] ) / ( properties_perimeter[ctn]*properties_perimeter[ctn] );
		
		printf("  ref_point X Y = %d  %d  \n", ctn,ref_point[ctn][0], ref_point[ctn][1]);
		printf("  area = %d  \n", properties_area[ctn]);
		printf("  perim = %d  \n", properties_perimeter[ctn]);
		printf("  compactness = %f  \n", properties_compactness[ctn]);
		fig_se.SetTitle("Region Figure");
		fig_se.Draw(region_gray[ctn]);
	}
	


	
	



	//	i.	zeroth-, first- and second-order moments (regular and centralized)
	double moment_r_val[region_n_defa][6]={0}; //[regions][element]
	double moment_c_val[region_n_defa][6]={0}; //[regions][element]
	const int m00=0,	m10=1,	m01=2,	m20=3,	m02=4,	m11=5;
	const int mu00=0, xbar=1, ybar=2,	mu11=3,	mu20=4,	mu02=5;
	
	ImgBgr img_out_f( img_loaded_image_g.Width(), img_loaded_image_g.Height() );
	//Set( &img_out_f, Bgr(0,0,0) );
	Convert(img_loaded_image_g, &img_out_f);



	//	iii.eccentricity (or elongatedness), using eigenvalues
	//	iv.	direction, using either eigenvectors (PCA) or the moments formula (they are equivalent)
	
	Point pt1 = (0,0) , pt2 = (0,0), pt3 = (0,0), pt4 = (0,0);

	//Covariance matrix of region
	MatDbl c_mat[region_n_defa];

	//Eigenvalues
	MatDbl eigenvalues[region_n_defa];
	
	//Eigenvectors
	MatDbl eigenvectors[region_n_defa];

	//Eccentricity
	double Eccentricity[region_n_defa]={0};
	
	//axis
	double major_axis[region_n_defa]={0};
	double minor_axis[region_n_defa]={0};

	//core
	double region_core[region_n_defa][2]={0};
		
	//Direction
	double direction[region_n_defa]={0};


	for(int ctn=0; ctn<region_n; ctn++)
	{
		if ( moment_compute(region_binary[ctn] , moment_r_val[ctn], moment_c_val[ctn])==1 ) 
		{	
			printf("Error: Wrong size input for Moment calculation \n"); 
			break;
		}
		
		//Xc = M10/M00;Yc = M01/M00
		region_core[ctn][0] = moment_r_val[ctn][m10] / moment_r_val[ctn][m00]+ properties_region_offset[ctn][0];
		region_core[ctn][1] = moment_r_val[ctn][m01] / moment_r_val[ctn][m00]+ properties_region_offset[ctn][1];



		//Covariance matrix of region
		c_mat[ctn].Reset(2,2);
		c_mat[ctn](0,0) = moment_c_val[ctn][mu20]/moment_c_val[ctn][mu00];
		c_mat[ctn](0,1) = moment_c_val[ctn][mu11]/moment_c_val[ctn][mu00];
		c_mat[ctn](1,0) = moment_c_val[ctn][mu11]/moment_c_val[ctn][mu00];
		c_mat[ctn](1,1) = moment_c_val[ctn][mu02]/moment_c_val[ctn][mu00];

		EigenSymm(c_mat[ctn], &eigenvalues[ctn], &eigenvectors[ctn]);


		//core
		double region_core[region_n_defa][2]={0};


		//Eccentricity
		double Eccentricity_val_temp=0;


		Eccentricity_val_temp= sqrt( 
								 (moment_c_val[ctn][mu20]-moment_c_val[ctn][mu02])
								*(moment_c_val[ctn][mu20]-moment_c_val[ctn][mu02]) 
								+ 4*moment_c_val[ctn][mu11]*moment_c_val[ctn][mu11]
							   );

		Eccentricity[ctn] = sqrt( 
									2 * Eccentricity_val_temp  / (moment_c_val[ctn][mu20] 
								  + moment_c_val[ctn][mu02] + Eccentricity_val_temp )
								);

		
		//major and minor axises
		major_axis[ctn]=sqrt( eigenvalues[ctn](0,0) );
		minor_axis[ctn]=sqrt( eigenvalues[ctn](0,1) );
		
		

		//Direction
		direction[ctn] = atan2(2*moment_c_val[ctn][mu11] , (moment_c_val[ctn][mu20]-moment_c_val[ctn][mu02]) )/2;
	}
		
	
	for(int ctn=0; ctn<region_n; ctn++)
	{
		printf("\n");
		printf("  Region %d: \n", ctn);
		//printf("Moment Regular in Region %d: \n", ctn);		
		//int m00=0,	m10=1,	m01=2,	m20=3,	m02=4,	m11=5;
		printf("  Moment first/regular m00 %f \n" , moment_r_val[ctn][m00] );
		printf("  Moment first/regular m10 %f \n" , moment_r_val[ctn][m10] );
		printf("  Moment first/regular m01 %f \n" , moment_r_val[ctn][m01] );
		printf("  Moment first/regular m20 %f \n" , moment_r_val[ctn][m20] );
		printf("  Moment first/regular m02 %f \n" , moment_r_val[ctn][m02] );
		printf("  Moment first/regular m11 %f \n" , moment_r_val[ctn][m11] );


		


		printf("\n");
		//printf("Moment Central in Region %d:", ctn);
		//int mu00=0, xbar=1, ybar=2,	mu11=3,	mu20=4,	mu02=5;					
		printf("  Moment second/Centeral mu00 %f \n" , moment_c_val[ctn][mu00] );
		printf("  Moment second/Centeral xbar %f \n" , moment_c_val[ctn][xbar] );
		printf("  Moment second/Centeral ybar %f \n" , moment_c_val[ctn][ybar] );
		printf("  Moment second/Centeral mu11 %f \n" , moment_c_val[ctn][mu11] );
		printf("  Moment second/Centeral mu20 %f \n" , moment_c_val[ctn][mu20] );
		printf("  Moment second/Centeral mu02 %f \n" , moment_c_val[ctn][mu02] );
		
		
		//core
		printf("  region_core X=%f  Y=%f \n" , region_core[ctn][0] ,region_core[ctn][1]  );
		
		
		//Ecc
		printf("  Eccentricity = %lf \n", Eccentricity[ctn]);
		//Dir
		printf("  direction = %lf \n", direction[ctn]);
				
		//printf("  Ecc in Region %d: \n", ctn);
		printf("  Center of region: X=%d, Y=%d: \n", properties_center_point[ctn][0] , properties_center_point[ctn][1] );
				
		printf("  Eigenvalus 0 = %f \n" , eigenvalues[ctn](0,0) );
		printf("  Eigenvalus 1 = %f \n" , eigenvalues[ctn](0,1) );

		/*printf("  Minor axis %f: \n", minor_axis[ctn]);
		printf("  Major axis %f: \n", major_axis[ctn]);*/

		printf("  Eigenvectors major [ %f , %f ]T  \n" , eigenvectors[ctn](0,0), eigenvectors[ctn](0,1) );
		printf("  Eigenvectors minor [ %f , %f ]T  \n\n" , eigenvectors[ctn](1,0), eigenvectors[ctn](1,1) );

		pt1.x = properties_center_point[ctn][0] -  major_axis[ctn] *  eigenvectors[ctn](0,0);
		pt1.y = properties_center_point[ctn][1] -  major_axis[ctn] *  eigenvectors[ctn](0,1);

		pt2.x = properties_center_point[ctn][0] +  major_axis[ctn] *  eigenvectors[ctn](0,0);
		pt2.y = properties_center_point[ctn][1] +  major_axis[ctn] * eigenvectors[ctn](0,1);


		pt3.x = properties_center_point[ctn][0] -  minor_axis[ctn] *  eigenvectors[ctn](1,0);
		pt3.y = properties_center_point[ctn][1] -  minor_axis[ctn] *  eigenvectors[ctn](1,1);

		pt4.x = properties_center_point[ctn][0] +  minor_axis[ctn] *  eigenvectors[ctn](1,0);
		pt4.y = properties_center_point[ctn][1] +  minor_axis[ctn] *  eigenvectors[ctn](1,1);
		
		
		DrawLine(pt1, pt2, &img_out_f, Bgr(255,0,0), 1);
		DrawLine(pt3, pt4, &img_out_f, Bgr(255,0,0), 1);
	
	}

	



	/*g.	Automatically classifies each piece of fruit into one of three categories:  apple, grapefruit, or banana, using a combination of these foreground properties.*/
	printf("  \n\n");
	printf("C. Region Catalog\n");


	int cata_region[region_n_defa]={0};
	const int unname_v=0, banana_v=1, apple_v=2, grapefruit_v=3;
	
	
	//cata diff regions
	for(int ctn=0; ctn<region_n; ctn++)
	{
		
		if( 
			  ( abs( eigenvalues[ctn](0,1) - eigenvalues[ctn](0,0) ) >150	)
			&&( properties_compactness[ctn] < 0.3)
		   )
		{ cata_region[ctn]= banana_v; printf("  Region %d is a banana\n", ctn); new_color[ctn]=Bgr(96,216,255); }
		
		else if(  (properties_area[ctn]< 5000) && ( eigenvalues[ctn](0,1) + eigenvalues[ctn](0,1) )/2 <350  )
		
		{ cata_region[ctn]= apple_v; printf("  Region %d is an apple\n", ctn);new_color[ctn]=Bgr(0,0,255);}
		
		else if(  (properties_area[ctn]> 5000 ) && ( eigenvalues[ctn](0,1) + eigenvalues[ctn](0,1) )/2 >350 )
	
		{ cata_region[ctn]= grapefruit_v; printf("  Region %d is a grapefruit\n", ctn);new_color[ctn]=Bgr(0,255,0); }

		else
	
		{ cata_region[ctn]= unname_v; printf("  Region %d is unknown\n", ctn);}

		

	}

    
	//outline cata regions
	if(  img_outline( region_gray, &img_out_f, new_color, properties_region_offset,region_n) == 0 )
	{
	
		printf("Region cata done\n");	
	
	}
	else
	{
	
		printf("Error: outline failure\n");
	
	}


	 

	/*h.	Detects the banana stem.  There is no one right way to do this.  A variety of techniques have been developed by students over the years 
		such as eroding until the stem disappears; rotating to align the major axis with the horizontal axis, then projecting onto the horizontal axis and finding the thinnest columns; 
		or finding the further point from the centroid then growing the region.  Feel free to be creative if you can think of another way.*/
	ImgBinary region_mid1;
	ImgBinary region_mid2;
	ImgBinary region_mid3;

	ImgGray region_fout[region_n_defa][2];
	//Figure fig_se[4];
	int yy=0;
	int dir=0;
	for(int ctn=0; ctn<region_n; ctn++)
	{
		if( cata_region[ctn] == banana_v )
		{
			region_mid1.Reset(region_binary[ctn].Width(),region_binary[ctn].Height() );
			region_mid2.Reset(region_binary[ctn].Width(),region_binary[ctn].Height() );
			

			region_mid1=region_binary[ctn];
			region_mid2=region_binary[ctn];

			Erode3x3    (region_mid1, &region_mid2);
			Erode3x3    (region_mid2, &region_mid1);
			Erode3x3    (region_mid1, &region_mid2);
			Erode3x3    (region_mid2, &region_mid1);
			

			

			Dilate3x3	(region_mid1, &region_mid2);
			Dilate3x3	(region_mid2, &region_mid1);
			Dilate3x3	(region_mid1, &region_mid2);
			Dilate3x3	(region_mid2, &region_mid1);


		
			region_mid3.Reset(region_binary[ctn].Width(),region_binary[ctn].Height());


			Xor(region_mid1, region_binary[ctn], &region_mid2); //get banana stem  b/w
			
			//Figure fig_se[4];
			//fig_se[0].Draw(region_mid2);
		
	

			Erode3x3Cross   (region_mid2, &region_mid1);//filter
			Dilate3x3Cross	(region_mid1, &region_mid2);//filter
			Erode3x3    (region_mid2, &region_mid1);// mid1 = mid2- outline



			//fig_se[1].Draw(region_mid1);
			
			
			Xor(region_mid1, region_mid2, &region_mid3); //get outline of banana stem
			
			//fig_se[3].Draw(region_mid1);
			//fig_se[2].Draw(region_mid3);
			

			int xx=0,yy=0;

			for(int y=0; y<region_binary[ctn].Height(); y++)
			{
				for(int x=0; x<region_binary[ctn].Width(); x++)
				{
					if( region_mid3(x,y) == 1 && region_gray[ctn](x,y)== 255)
					{
						//R215 G0 B64
						img_out_f(x+properties_region_offset[ctn][0],y+properties_region_offset[ctn][1]) = Bgr(64,0,215);
								
					}



				}//for x
			}//for y


		}//if cata
		
	}// if ctn

	fig_out_f.Draw(img_out_f);	

	EventLoop();
	return 0;
}
