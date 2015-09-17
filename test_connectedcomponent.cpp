#include <afxwin.h>  // necessary for MFC to work properly
#include "Homework.h"
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
	printf("a.Start: n");

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
	//Erode3x3 (img_low_threshold_g, &img_tr_tmp);
	//Erode3x3  (img_tr_tmp,&img_low_threshold_g);


	Dilate3x3    (img_low_threshold_g, &img_tr_tmp);
	Dilate3x3    (img_tr_tmp, &img_low_threshold_g);
	//Dilate3x3    (img_low_threshold_g, &img_tr_tmp);
	//Dilate3x3    (img_tr_tmp, &img_low_threshold_g);

	fig_low_threshold.Draw(img_low_threshold_g);


	/*e.	Runs connected components (either the classic union-find algorithm or by repeated applications of floodfill) to detect and count the foreground regions 
		in the cleaned, thresholded image, distinguishing them from the background.  */
	
	//ImgBinary img_low_threshold_g;
	ImgInt labels;
	std::vector< ConnectedComponentProperties<ImgGray::Pixel> > reg;  		// pixel type must match input image type
	reg.reserve(18);
	int region_num = ConnectedComponents4(img_low_threshold_g, &labels, &(reg));

	std::vector<int> num_tar_region;
	for(int i=0 ; i<region_num ; i++)
	{
		if( reg[i].npixels > 200 && reg[i].value==255)
		{
			num_tar_region.push_back(i);
			printf("\nConneccted reg[%d].npixels= %d ",i,reg[i].npixels);
			printf("Seed point x=%d  y=%d \n",reg[i].pixel.x, reg[i].pixel.y);
			//printf("Value= %d \n",reg[i].value);
		}
	}
	
	Figure fig_after_connect;
	fig_after_connect.SetTitle("After connectcomponent");
	fig_after_connect.Draw(	labels);

	/* for next, based on the seed point, performing floodfill cuts off the region from the origin img.*/

	EventLoop();
	return 0;
}
