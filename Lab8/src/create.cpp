#include "utils.hpp"

using namespace cv;


/**
 * @brief Create a board object
 * 
 * @param out out file
 * @param markersX Number of markers in X direction
 * @param markersY Number of markers in Y direction
 * @param markerLength Marker side length (in pixels)
 * @param markerSeparation Separation between two consecutive markers in the grid (in pixels)
 * @param margins Margins size (in pixels). Default is marker separation (-s)
 * @param dictionaryId dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
    "DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
    "DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
    "DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16
 * @param showImage  show generated image
 */
void create_board(String out,
                  int markersX,
                  int markersY,
                  int markerLength,
                  int markerSeparation,
                  int margins,
                  int dictionaryId,
                  bool showImage)
{
    int borderBits = 1;
    
    Size imageSize;
    imageSize.width = markersX * (markerLength + markerSeparation) - markerSeparation + 2 * margins;
    imageSize.height =
        markersY * (markerLength + markerSeparation) - markerSeparation + 2 * margins;

    auto dictionary = cv::aruco::getPredefinedDictionary(dictionaryId);
    aruco::GridBoard board(Size(markersX, markersY),
                           float(markerLength),
                           float(markerSeparation),
                           dictionary);

    // show created board
    //! [aruco_generate_board_image]
    Mat boardImage;
    board.generateImage(imageSize, boardImage, margins, borderBits);
    //! [aruco_generate_board_image]

    if (showImage)
    {
        imshow("board", boardImage);
        waitKey(0);
    }

    imwrite(out, boardImage);
}