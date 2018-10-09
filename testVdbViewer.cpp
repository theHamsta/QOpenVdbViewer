#include <openvdb/openvdb.h>
#include "QOpenVdbViewer.hpp"
#include <qapplication.h>



int main( int argc, char** argv )
{
	QApplication application( argc, argv );



	QOpenVdbViewer viewer;
	viewer.setWindowTitle( "simpleViewer" );

	viewer.show();
	openvdb::io::File file( "/local/xu29mapu/projects/gvdb-voxels/source/shared_assets/bunny.vdb" );
	file.open();

	try {
		auto grids = file.getGrids();

		if ( grids->size() ) {
			openvdb::GridBase::Ptr grid = grids->at( 0 );
			viewer.setGrid( grid );
		}
	} catch ( std::exception& err ) {
		std::cerr << err.what()   << std::endl;
	}

	return application.exec();
}
