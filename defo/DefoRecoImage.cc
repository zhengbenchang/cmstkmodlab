
#include "DefoRecoImage.h"

///
/// image reconstructor class:
/// identify dots, return reconstructed center points
///
DefoRecoImage::DefoRecoImage() {

  DefoConfigReader cfgReader( "defo.cfg" );

  step1Threshold_ = cfgReader.getValue<int>( "STEP1_THRESHOLD" );
  step2Threshold_ = cfgReader.getValue<int>( "STEP2_THRESHOLD" );
  step3Threshold_ = cfgReader.getValue<int>( "STEP3_THRESHOLD" );
  halfSquareWidth_ = cfgReader.getValue<int>( "HALF_SQUARE_WIDTH" );
  squareSeedOffsetX_ = cfgReader.getValue<int>( "SQUARE_SEED_OFFSET_X" );
  squareSeedOffsetY_ = cfgReader.getValue<int>( "SQUARE_SEED_OFFSET_Y" );
  reddishnessThreshold_ = cfgReader.getValue<double>( "REDDISHNESS_THRESHOLD" );
  debugLevel_ = cfgReader.getValue<unsigned int>( "DEBUG_LEVEL" );

}


///
/// point finding & reconstruction in a raw image;
/// returns: 
///  1. vector of DefoPoints, one for each dot reconstructed in theImage
///  2. a copy of the original image with square & cross showing reco results
///
std::pair<DefoPointCollection,DefoRawImage> DefoRecoImage::reconstruct( DefoRawImage& theImage ) {

  DefoRawImage copyOfTheRawImage( theImage );
  DefoPointCollection outputCollection;

  std::pair<DefoPointCollection,DefoRawImage> theOutput
    = std::make_pair<DefoPointCollection,DefoRawImage>( outputCollection, copyOfTheRawImage );

  std::pair<double,double> imageWidth( theImage.getImage().width(), theImage.getImage().height() );

  if( debugLevel_ >= 2 ) std::cout << " [DefoRecoImage::reconstruct] =2= Image has: "
				   << imageWidth.first << " x " << imageWidth.second << " pixels" << std::endl;
  
  // scan all pixels, but keep <halfSquareWidth+1> distance from borders
  for( unsigned int xIt = halfSquareWidth_ + 1; xIt < imageWidth.first - halfSquareWidth_ - 1; ++xIt ) {
    for( unsigned int yIt = halfSquareWidth_ + 1; yIt < imageWidth.second - halfSquareWidth_ - 1; ++yIt ) {

      // do not consider areas around points already picked up
      if( forbiddenAreas_.isInside( DefoPoint( xIt, yIt ) ) ) continue;
      
      // check if this pixel has a grayscale adc value above step1 threshold
      if( qGray( theImage.getImage().pixel( xIt, yIt ) ) > step1Threshold_ ) {
      
	// have an initial seed, check average amplitude
	// of some more pixels ahead
	double theProbe = 0.;
	theProbe += qGray( theImage.getImage().pixel( xIt + 2, yIt + 2 ) );
	theProbe += qGray( theImage.getImage().pixel( xIt + 2, yIt + 3 ) );
	theProbe += qGray( theImage.getImage().pixel( xIt + 3, yIt + 2 ) );
	theProbe += qGray( theImage.getImage().pixel( xIt + 3 ,yIt + 3 ) );
 	theProbe /= 4.;

	// check if the summed value is above step2 threshold
	if( theProbe < step2Threshold_ ) continue;

	// have a "cornfirmed" seed here..
	// now reconstruct its position;
	// do this recursively 3 times for improving the coordinates

	// iteration 1
	DefoPoint intermediatePoint = averageSquare( theImage.getImage(), DefoPoint( xIt, yIt ), 0 );

	// check if this point is still in range, otherwise drop it
	if( intermediatePoint.getX() < halfSquareWidth_ + 1 ||
	    intermediatePoint.getY() < halfSquareWidth_ + 1 ||
	    intermediatePoint.getX() > imageWidth.first  - halfSquareWidth_ - 1 ||
	    intermediatePoint.getY() > imageWidth.second - halfSquareWidth_ - 1    ) {
	  
	  if( debugLevel_ >= 3 ) std::cout << " [DefoRecoImage::reconstruct] =3= (Pos1) Point: x: "
					   << xIt << " y: " << yIt << " jumped to: x: "
					   << intermediatePoint.getX() << " y: " << intermediatePoint.getY()
					   << " . Dropping it." << std::endl;
	  continue;
	}


	// determine if it's a red LED before the next iterations
	// so we can later (iteration 3) draw the cross in the appropriate color
	const double reddishness = calculateReddishness( theImage.getImage(), DefoPoint( xIt, yIt ) );
	if( reddishness > reddishnessThreshold_ ) {
	  intermediatePoint.setRed( true );
	  if( debugLevel_ >= 3 ) std::cout << " [DefoRecoImage::reconstruct] =3= Identified red point: x: "
					   << intermediatePoint.getX() << " y: " << intermediatePoint.getY() 
					   << " reddishness: " << reddishness << std::endl;
	}

	// iteration 2
	intermediatePoint = averageSquare( theImage.getImage(), intermediatePoint, 0 );

	// check if this point is still in range, otherwise drop it
	if( intermediatePoint.getX() < halfSquareWidth_ + 1 ||
	    intermediatePoint.getY() < halfSquareWidth_ + 1 ||
	    intermediatePoint.getX() > imageWidth.first  - halfSquareWidth_ - 1 ||
	    intermediatePoint.getY() > imageWidth.second - halfSquareWidth_ - 1    ) {
	  
	  if( debugLevel_ >= 3 ) std::cout << " [DefoRecoImage::reconstruct] =3= (Pos2) Point: x: "
					   << xIt << " y: " << yIt << " jumped to: x: "
					   << intermediatePoint.getX() << " y: " << intermediatePoint.getY()
					   << " . Dropping it." << std::endl;
	  continue;
	}

	// iteration 3;
	// we draw the point now, even if it could be dropped later
        intermediatePoint = averageSquare( theImage.getImage(), intermediatePoint, &(theOutput.second.getImage()) );


	// check if this point is still in range, otherwise drop it
	if( intermediatePoint.getX() < halfSquareWidth_ + 1 ||
	    intermediatePoint.getY() < halfSquareWidth_ + 1 ||
	    intermediatePoint.getX() > imageWidth.first  - halfSquareWidth_ - 1 ||
	    intermediatePoint.getY() > imageWidth.second - halfSquareWidth_ - 1    ) {
	  
	  if( debugLevel_ >= 3 ) std::cout << " [DefoRecoImage::reconstruct] =3= (Pos3) Point: x: "
					   << xIt << " y: " << yIt << " jumped to: x: "
					   << intermediatePoint.getX() << " y: " << intermediatePoint.getY()
					   << " . Dropping it." << std::endl;
	  continue;
	}

	theOutput.first.push_back( intermediatePoint );

	if( debugLevel_ >= 3 ) std::cout << " [DefoRecoImage::reconstruct] =3= Reconstructed point at: x: "
					 << intermediatePoint.getX() << " y: " << intermediatePoint.getY() << std::endl;

	// save square around this point as already tagged
	forbiddenAreas_.push_back( DefoSquare( intermediatePoint, halfSquareWidth_ ) );

      }

    }
  }

  forbiddenAreas_.makeEmpty();

  return theOutput;

}



///
/// around a given seed point (theSeed), create a square (width=halfSquareWidth),
/// find center-of-gravity and return this as the reconstructed point position;
/// if 3rd parameter is nonzero, draw rectangle & cross in that image
///
DefoPoint DefoRecoImage::averageSquare( QImage const& theImage, DefoPoint const& theSeed, QImage* imageForDrawing ) const {
  
  DefoPoint outputPoint( 0., 0. );
  outputPoint.setRed( theSeed.isRed() ); // copy "red" flag
  int squareSummedAmplitude = 0;

  // create a square of pixels round the seed
  DefoSquare theSquare( theSeed, halfSquareWidth_ );

  // loop all pixels in the square
  for( DefoSquare::iterator it = theSquare.begin(); it <= theSquare.end(); ++it ) {

    // retrieve grayscale value of that pixel
    const int grayscaleValue = qGray( theImage.pixel( it.getPoint().getPixX(), it.getPoint().getPixY() ) );

    // sum amplitude of all pixels above step3 threshold
    // for center-of-gravity
    if( grayscaleValue > step3Threshold_ ) {
      outputPoint += ( DefoPoint( it.getPoint().getX(), it.getPoint() .getY() ) * grayscaleValue );
      squareSummedAmplitude += grayscaleValue;
    }

  }

  // divide by total sum for center-of-gravity
  if( squareSummedAmplitude > 0 ) outputPoint /= squareSummedAmplitude;

  if( imageForDrawing ) {

    // draw square
    theSquare.draw( *imageForDrawing );
    
    // set draw color: blue if white point / green if red point
    unsigned int colorIndex = qRgb( 255, 0, 0 );
    if( outputPoint.isRed() ) colorIndex = qRgb( 0, 255, 0 ); // draw green cross if red point

    // draw cross @ output point
    for( int px = -2; px <= 2; ++px ) imageForDrawing->setPixel( outputPoint.getPixX() + px, outputPoint.getPixY(), colorIndex );
    for( int py = -2; py <= 2; ++py ) imageForDrawing->setPixel( outputPoint.getPixX(), outputPoint.getPixY() + py, colorIndex );

  }  

  return outputPoint;

}



///
/// determines the "reddishness" of a point,
/// i.e. the average ratio red/cyan of the color of its pixels
/// to determine if this is the reflection of a red LED
///
double DefoRecoImage::calculateReddishness( QImage const& theImage, DefoPoint const& theSeed ) const {

  double summedReddishness = 0.;
  unsigned int nPixel = 0;

  // create a square of pixels round the seed
  DefoSquare theSquare( theSeed, halfSquareWidth_ );

  // loop all pixels in the square
  for( DefoSquare::iterator it = theSquare.begin(); it <= theSquare.end(); ++it ) {

    // retrieve grayscale value of that pixel
    const int grayscaleValue = qGray( theImage.pixel( it.getPoint().getPixX(), it.getPoint().getPixY() ) );

    // determine "reddishness" of this pixel
    if( grayscaleValue ) {

      const double cyan = (
	  qBlue( theImage.pixel( it.getPoint().getPixX(), it.getPoint().getPixY() ) )
	+ qGreen( theImage.pixel( it.getPoint().getPixX(), it.getPoint().getPixY() ) )
      ) / 2.;

      if( cyan ) {
	summedReddishness += qRed( theImage.pixel( it.getPoint().getPixX(), it.getPoint().getPixY() ) ) / cyan;
	nPixel++;
      }
    }


  }

  if( nPixel ) return( summedReddishness / nPixel );
  else {
    std::cerr << " [DefoRecoImage::calculateReddishness] ** ERROR: no pixels > 0 in point window" << std::endl;
    throw;
  }
  
}
