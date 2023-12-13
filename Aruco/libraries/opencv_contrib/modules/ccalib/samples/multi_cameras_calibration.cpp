#include "opencv2/ccalib/omnidir.hpp"
#include "opencv2/ccalib/multicalib.hpp"
#include "opencv2/ccalib/randpattern.hpp"

using namespace std;
using namespace cv;

const char * usage =
"\n example command line for multi-camera calibration by using random pattern \n"
"   multi_cameras_calibration -nc 5 -pw 800 -ph 600 -ct 1 -fe 0 -nm 25 -v 0 multi_camera_omnidir.xml \n"
"\n"
" the file multi_camera_omnidir.xml is generated by imagelist_creator as \n"
" imagelist_creator multi_camera_omnidir.xml *.* \n"
" note the first filename in multi_camera_omnidir.xml is the pattern, the rest are photo names,\n"
" photo names should be in form of cameraIdx-timestamp.*, and cameraIdx starts from 0";

static void help()
{
    printf("\n This is a sample for multi-camera calibration, so far it only support random pattern,\n"
           "see randomPattern.hpp for detail. Pinhole and omnidirectional cameras are both supported, \n"
           "for omnidirectional camera, see omnidir.hpp for detail.\n"
           "Usage: mutiCamCalib \n"
           "    -nc <num_camera> # number of cameras \n"
           "    -pw <pattern_width> # physical width of random pattern \n"
           "    -ph <pattern_height> # physical height of random pattern \n"
           "    -ct <camera_type> # camera type, 0 for pinhole and 1 for omnidirectional \n"
           "    -fe # whether show feature extraction\n"
           "    -nm # number of minimal matches of an image \n"
		   "	-v # whether show verbose information \n"
           "    input_data # text file with pattern file names and a list of photo names, the file is generated by imagelist_creator \n");
    printf("\n %s", usage);
}


int main(int argc, char** argv)
{
    float patternWidth = 0.0f, patternHeight = 0.0f;
    int nCamera = 0, nMiniMatches = 0, cameraType = 0;
    const char* outputFilename = "multi-camera-results.xml";
    const char* inputFilename = 0;
    int showFeatureExtraction = 0, verbose = 0;
    if (argc < 2)
    {
        help();
        return 1;
    }

    for (int i = 1; i < argc; ++i)
    {
        const char* s = argv[i];
        if (strcmp( s, "-nc") == 0)
        {
            if (sscanf( argv[++i], "%u", &nCamera) != 1 || nCamera <= 0)
            {
                return fprintf(stderr, "Invalid number of cameras \n"), -1;
            }
        }
        else if ( strcmp( s, "-pw" ) == 0 )
        {
            if (sscanf( argv[++i], "%f", &patternWidth) != 1 || patternWidth <=0 )
            {
                return fprintf(stderr, "Invalid pattern width \n"), -1;
            }
        }
        else if ( strcmp( s, "-ph" ) == 0 )
        {
            if (sscanf( argv[++i], "%f", &patternHeight) != 1 || patternHeight <=0 )
            {
                return fprintf(stderr, "Invalid pattern height \n"), -1;
            }
        }
        else if ( strcmp( s, "-ct" ) == 0 )
        {
            if (sscanf( argv[++i], "%u", &cameraType) != 1 || (cameraType !=0 && cameraType !=1 && cameraType !=2) )
            {
                return fprintf(stderr, "Invalid camera type, 0 for pinhole and 1 for omnidirectional \n"), -1;
            }
        }
        else if ( strcmp( s, "-fe" ) == 0 )
        {
            if (sscanf( argv[++i], "%u", &showFeatureExtraction) != 1 || (showFeatureExtraction !=1 && showFeatureExtraction !=0) )
            {
                return fprintf(stderr, "Not bool value, set to 0 or 1 \n"), -1;
            }
        }
        else if ( strcmp( s, "-nm" ) == 0 )
        {
            if (sscanf( argv[++i], "%u", &nMiniMatches) != 1 || nMiniMatches <=0 )
            {
                return fprintf(stderr, "Invalid number of minimal matches \n"), -1;
            }
        }
		else if ( strcmp( s, "-v" ) == 0 )
		{
			if (sscanf( argv[++i], "%u", &verbose) != 1 || (verbose !=1 && verbose !=0) )
			{
				return fprintf(stderr, "verbose is not bool value, set to 0 or 1 \n"), -1;
			}
		}
        else if( s[0] != '-')
        {
            inputFilename = s;
        }
        else
        {
            return fprintf( stderr, "Unknown option %s\n", s ), -1;
        }
    }

    // do multi-camera calibration
    multicalib::MultiCameraCalibration multiCalib(cameraType, nCamera, inputFilename, patternWidth, patternHeight, verbose, showFeatureExtraction, nMiniMatches);

    multiCalib.loadImages();
    multiCalib.initialize();
    multiCalib.optimizeExtrinsics();
    // the above three lines can be replaced by multiCalib.run();


	multiCalib.writeParameters(outputFilename);
}