#pragma once
#pragma push_macro("Q_FOREACH")
#pragma push_macro("foreach")
#undef Q_FOREACH
#undef foreach
#include <openvdb/openvdb.h>
#include <gvdb.h>
#pragma pop_macro("Q_FOREACH")
#pragma pop_macro("foreach")
#include <QGLViewer/qglviewer.h>
#include <map>
#include <string>
#include <array>
#include <memory>
#include <QWidget>

enum class VolumeRenderingMode { Levelset, Transparent };

// TODO: make singleton if VolumeGVDB must be a singleton
class QOpenVdbViewer : public QGLViewer
{
	Q_OBJECT
	public:
		explicit QOpenVdbViewer( QWidget* parent = nullptr );
		QOpenVdbViewer( QOpenVdbViewer&& ) = delete;
		QOpenVdbViewer( const QOpenVdbViewer& ) = delete;
		QOpenVdbViewer& operator=( QOpenVdbViewer&& ) = delete;
		QOpenVdbViewer& operator=( const QOpenVdbViewer& ) = delete;
		virtual ~QOpenVdbViewer();

	public slots:
		void setBackgroundColor( std::array<float, 4> color );
		void setBackgroundColor( float r, float g, float b, float alpha = 1.f );

		void setVolumeRenderingMode( VolumeRenderingMode renderingMode );
		void setMinMax( float min, float max ) { m_min = min; m_max = max; m_updateMinMax = true; }

		// 		void clearGrids() { m_grids.clear(); updateGrids(); }
		// 		void setGrid( openvdb::FloatGrid::Ptr grid, std::string identifier ) { m_grids[identifier] = grid; updateGrids(); }
		inline void setGrid( openvdb::GridBase::Ptr grid ) { m_grid = grid; updateGrids();  }

		std::shared_ptr<VolumeGVDB> volumeGVDB() { return m_gvdb; }

		virtual void resizeEvent( QResizeEvent* e ) override;
		inline VolumeRenderingMode volumeRenderingMode() const { return m_volumeRenderingMode; }

	protected:
		virtual void draw();
		virtual void init();
		virtual QString helpString() const;
		void updateRenderingMode();
		void updateGrids();

		std::shared_ptr<VolumeGVDB> m_gvdb;
		GLuint m_renderTexture = -1;
		openvdb::GridBase::Ptr m_grid = nullptr;
		VolumeRenderingMode m_volumeRenderingMode = VolumeRenderingMode::Transparent;
		float m_min, m_max;
		bool m_updateMinMax = false;
		// 		std::map<std::string, openvdb::GridBase::Ptr> m_grids;
};
