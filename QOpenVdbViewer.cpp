#include "QOpenVdbViewer.hpp"
#include "QGLViewer/vec.h"
#include "QResizeEvent"
#include "NotImplementedException.hpp"


QOpenVdbViewer::QOpenVdbViewer(QWidget* parent)
	: QGLViewer(parent)
{
	m_gvdb = std::make_shared<VolumeGVDB>();
	m_gvdb->SetVerbose(true);
	m_gvdb->SetCudaDevice(GVDB_DEV_FIRST);
	//     if(m_gvdb->is
	m_gvdb->Initialize();
	init();
}

QOpenVdbViewer::~QOpenVdbViewer()
{
}

void QOpenVdbViewer::draw()
{

	// m_gvdb->getScene()->SetCutoff ( 0.005f, 0.005f, 0.0f );
	// 	m_gvdb->getScene()->LinearTransferFunc( 0.00f, 0.25f, Vector4DF( 0, 0, 0, 0 ), Vector4DF( 1, 0, 0, 0.05f ) );
	// 	m_gvdb->getScene()->LinearTransferFunc( 0.25f, 0.50f, Vector4DF( 1, 0, 0, 0.05f ), Vector4DF( 1, .5f, 0, 0.1f ) );
	// 	m_gvdb->getScene()->LinearTransferFunc( 0.50f, 0.75f, Vector4DF( 1, .5f, 0, 0.1f ), Vector4DF( 1, 1, 0, 0.15f ) );
	// 	m_gvdb->getScene()->LinearTransferFunc( 0.75f, 1.00f, Vector4DF( 1, 1, 0, 0.15f ), Vector4DF( 1, 1, 1, 0.2f ) );
	// 	m_gvdb->CommitTransferFunc ();
	//     m_gvdb->getScene()->SetSteps ( 0.25, 16, 0.25 );               // Set raycasting steps
	//     m_gvdb->getScene()->SetExtinct ( -1.0f, 1.5f, 0.0f );      // Set volume extinction
	//     m_gvdb->getScene()->SetVolumeRange ( 0.0f, 1.0f, -1.0f );  // Set volume value range (for a level set)
	// m_gvdb->getScene()->LinearTransferFunc ( 0.00f, 0.25f, Vector4DF( 1, 1, 0, 0.05f ), Vector4DF( 1, 1, 0, 0.03f ) );
	// m_gvdb->getScene()->LinearTransferFunc ( 0.25f, 0.50f, Vector4DF( 1, 1, 1, 0.03f ), Vector4DF( 1, 0, 0, 0.02f ) );
	// m_gvdb->getScene()->LinearTransferFunc ( 0.50f, 0.75f, Vector4DF( 1, 0, 0, 0.02f ), Vector4DF( 1, .5f, 0, 0.01f ) );
	// m_gvdb->getScene()->LinearTransferFunc ( 0.75f, 1.00f, Vector4DF( 1, .5f, 0, 0.01f ), Vector4DF( 0, 0, 0, 0.005f ) );
	// //     m_gvdb->getScene()->SetBackgroundClr ( 0, 0, 0, 1 );
	// m_gvdb->CommitTransferFunc ();
	if (m_updateNecessary) {
		m_gvdb->getScene()->SetSteps(m_rayCastingStepSize, 16, 16);                // Set raycasting steps
		// m_gvdb->getScene()->SetExtinct( -1.0f, 1.5f, 0.0f );        // Set volume extinction
		// m_gvdb->getScene()->SetVolumeRange ( 0.1f, n:, .5f );  // Set volume value range
		m_gvdb->getScene()->SetVolumeRange(0.1f, m_min, m_max);     // Set volume value range (for a level set)
		m_gvdb->getScene()->SetCutoff(m_cutoff[0], m_cutoff[1], m_cutoff[2]);
		// m_gvdb->getScene()->LinearTransferFunc ( 0, 1000, Vector4DF( 1, .5f, 0, 0.01f ), Vector4DF( 0, 0, 0, 0.005f ) );
		// m_gvdb->CommitTransferFunc ();
		m_updateNecessary = false;

	}

	if (m_grid) {
		updateRenderingMode();
		m_gvdb->ReadRenderTexGL(0, m_renderTexture);
	}

	GLfloat modelViewMatrix[16];
	GLfloat modelViewProjectionMatrix[16];
	GLfloat projectionMatrix[16];
	this->camera()->getModelViewMatrix(modelViewMatrix);
	this->camera()->getModelViewProjectionMatrix(modelViewProjectionMatrix);
	this->camera()->getProjectionMatrix(projectionMatrix);
	qglviewer::Vec position = this->camera()->position();

	nvdb::Camera3D* camera = m_gvdb->getScene()->getCamera();
	//    camera->setMatrices(modelViewMatrix, projectionMatrix,
	//                        Vector3DF{static_cast<float>(position[0]),static_cast<float>(position[1]),static_cast<float>(position[2])});
	camera->setMatrices(modelViewMatrix, projectionMatrix,
						Vector3DF {50.f, 00.f, 00.f});


	// glDisable( GL_DEPTH_TEST );
	// glDisable( GL_CULL_FACE );
	// glDepthMask( GL_FALSE );

	startScreenCoordinatesSystem();
	constexpr int x_pos = 0;
	constexpr int y_pos = 0;
	glBindTexture(GL_TEXTURE_2D, m_renderTexture);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(x_pos, y_pos, 0.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(x_pos, y_pos + geometry().height(), 0.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(x_pos + geometry().width(), y_pos + geometry().height(), 0.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(x_pos + geometry().width(), y_pos, 0.0f);
	glEnd();
	stopScreenCoordinatesSystem();

}

static void checkGL(const char* msg)
{
	GLenum errCode;
	errCode = glGetError();

	if (errCode != GL_NO_ERROR) {
		printf("%s, ERROR: 0x%x\n", msg, errCode);
	}
}





static void createScreenQuadGL(GLuint* texture, int width, int height)
{
	if (*texture == -1) glDeleteTextures(1, texture);

	glGenTextures(1,  texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	checkGL("glBindTexture (createScreenQuadGL)");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	checkGL("glTexImage2D (createScreenQuadGL)");
	glBindTexture(GL_TEXTURE_2D, 0);
}

void QOpenVdbViewer::init()
{
	// 	m_gvdb->getScene()->SetSteps ( 0.25, 16, 0.25 );               // Set raycasting steps
	// 	m_gvdb->getScene()->SetExtinct ( -1.0f, 1.5f, 0.0f );      // Set volume extinction
	// 	m_gvdb->getScene()->SetVolumeRange ( 0.0f, 1.0f, -1.0f );  // Set volume value range (for a level set)
	// 	m_gvdb->getScene()->LinearTransferFunc ( 0.00f, 0.25f, Vector4DF( 1, 1, 0, 0.05f ), Vector4DF( 1, 1, 0, 0.03f ) );
	// 	m_gvdb->getScene()->LinearTransferFunc ( 0.25f, 0.50f, Vector4DF( 1, 1, 1, 0.03f ), Vector4DF( 1, 0, 0, 0.02f ) );
	// 	m_gvdb->getScene()->LinearTransferFunc ( 0.50f, 0.75f, Vector4DF( 1, 0, 0, 0.02f ), Vector4DF( 1, .5f, 0, 0.01f ) );
	// 	m_gvdb->getScene()->LinearTransferFunc ( 0.75f, 1.00f, Vector4DF( 1, .5f, 0, 0.01f ), Vector4DF( 0, 0, 0, 0.005f ) );
	// 	    m_gvdb->getScene()->SetBackgroundClr ( 0, 0, 0, 1 );
	// 	m_gvdb->CommitTransferFunc ();

	// Restore previous viewer state.
	restoreStateFromFile();

	// 	char scnpath[1024];
	//
	// 	if ( !m_gvdb->getScene()->FindFile ( "/local/xu29mapu/projects/gvdb-voxels/source/shared_assets/" "bunny.vdb", scnpath ) ) {
	// 		gprintf ( "Cannot find vdb file.\n" );
	// 		gerror();
	// 	}

	m_gvdb->SetChannelDefault(16, 16, 16);

	// 	if ( !m_gvdb->LoadVDB ( scnpath ) ) {                  // Load OpenVDB format
	// 		gerror();
	// 	}



	Camera3D* cam = new Camera3D;
	cam->setFov(50.0);
	cam->setOrbit(Vector3DF(20, 30, 0), Vector3DF(0, 0, 0), 700, 1.0);
	m_gvdb->getScene()->SetCamera(cam);

	Light* lgt = new Light;
	lgt->setOrbit(Vector3DF(299, 57.3f, 0), Vector3DF(132, -20, 50), 200, 1.0);
	m_gvdb->getScene()->SetLight(0, lgt);
	int w = 1024;// this->geometry().width();
	int h = 960; //this->geometry().height();

	m_gvdb->AddRenderBuf(0, w, h, 4);


	auto gvdbCamera = m_gvdb->getScene()->getCamera();
	auto cameraPosition = gvdbCamera->getPos();
	//     auto cameraOrientation = gvdbCamera->getVi();
	//     this->camera()->setPosition( {cameraPosition.x,cameraPosition.y, cameraPosition.z});
	//     this->camera()->setOrientation( {cameraPosition[0],cameraPosition[1], cameraPosition[2]});

	createScreenQuadGL(&m_renderTexture, w, h);
}

void QOpenVdbViewer::resizeEvent(QResizeEvent* e)
{
	QGLWidget::resizeEvent(e);

	const int width = e->size().width();
	const int height = e->size().height();

	createScreenQuadGL(&m_renderTexture, width, height);
	m_gvdb->ResizeRenderBuf(0, width, height, 4);
}

void QOpenVdbViewer::setVolumeRenderingMode(VolumeRenderingMode renderingMode)
{
	m_volumeRenderingMode = renderingMode;
	update();
}


QString QOpenVdbViewer::helpString() const
{
	QString text("<h2>S i m p l e V i e w e r</h2>");
	text += "Use the mouse to move the camera around the object. ";
	text += "You can respectively revolve around, zoom and translate with the "
			"three mouse buttons. ";
	text += "Left and middle buttons pressed together rotate around the camera "
			"view direction axis<br><br>";
	text += "Pressing <b>Alt</b> and one of the function keys "
			"(<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
	text += "Simply press the function key again to restore it. Several "
			"keyFrames define a ";
	text += "camera path. Paths are saved when you quit the application and "
			"restored at next start.<br><br>";
	text +=
		"Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
	text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save "
			"a snapshot. ";
	text += "See the <b>Keyboard</b> tab in this window for a complete shortcut "
			"list.<br><br>";
	text += "Double clicks automates single click actions: A left button double "
			"click aligns the closer axis with the camera (if close enough). ";
	text += "A middle button double click fits the zoom of the camera and the "
			"right button re-centers the scene.<br><br>";
	text += "A left button double click while holding right button pressed "
			"defines the camera <i>Revolve Around Point</i>. ";
	text += "See the <b>Mouse</b> tab and the documentation web pages for "
			"details.<br><br>";
	text += "Press <b>Escape</b> to exit the viewer.";
	return text;
}

void QOpenVdbViewer::updateGrids()
{
	if (m_grid) {
		m_gvdb->LoadVDB(m_grid);
	}
}

void QOpenVdbViewer::setBackgroundColor(std::array< float, int(4) > color)
{
	m_gvdb->getScene()->SetBackgroundClr(color[0], color[1], color[2], color[3]);
}

void QOpenVdbViewer::setBackgroundColor(float r, float g, float b, float alpha)
{
	m_gvdb->getScene()->SetBackgroundClr(r, g, b, alpha);
}

void QOpenVdbViewer::updateRenderingMode()
{
	switch (m_volumeRenderingMode) {
		case VolumeRenderingMode::Voxel: {
				m_gvdb->Render(SHADE_VOXEL, 0, 0);
				break;
			}

		case VolumeRenderingMode::Section2D: {
				m_gvdb->Render(SHADE_SECTION2D, 0, 0);
				break;
			}

		case VolumeRenderingMode::Section3D: {
				m_gvdb->Render(SHADE_SECTION3D, 0, 0);
				break;
			}

		case VolumeRenderingMode::EmptySkip: {
				m_gvdb->Render(SHADE_EMPTYSKIP, 0, 0);
				break;
			}

		case VolumeRenderingMode::Trilinear: {
				m_gvdb->Render(SHADE_TRILINEAR, 0, 0);
				break;
			}

		case VolumeRenderingMode::Tricubic: {
				m_gvdb->Render(SHADE_TRICUBIC, 0, 0);
				break;
			}

		case VolumeRenderingMode::LevelSet: {
				m_gvdb->Render(SHADE_LEVELSET, 0, 0);
				break;
			}

		case VolumeRenderingMode::Volume: {
				m_gvdb->Render(SHADE_VOLUME, 0, 0);
				break;
			}

		case VolumeRenderingMode::Off: {
				m_gvdb->Render(SHADE_OFF, 0, 0);
				break;
			}

		default:
			throw NotImplementedException();

	}
}

// void QOpenVdbViewer::setMinMax( float min, float max )
// {
// 	if ( m_gvdb->getScene() ) {
// 	}
// }
