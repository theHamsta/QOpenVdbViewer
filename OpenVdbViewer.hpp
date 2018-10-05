#pragma once

#include <openvdb/openvdb.h>
#include <gvdb.h>
#include <QGLViewer/qglviewer.h>
#include <map>
#include <string>
#include <array>

enum class VolumeRenderingMode { Levelset, Transparent };

// TODO: make singleton if VolumeGVDB must be a singleton
class OpenVdbViewer : public QGLViewer
{
	public:
		OpenVdbViewer();
		OpenVdbViewer( OpenVdbViewer&& ) = delete;
		OpenVdbViewer( const OpenVdbViewer& ) = delete;
		OpenVdbViewer& operator=( OpenVdbViewer&& ) = delete;
		OpenVdbViewer& operator=( const OpenVdbViewer& ) = delete;
		virtual ~OpenVdbViewer();

		inline void setBackgroundColor( std::array<float, 4> color );
		inline void setBackgroundColor( float r, float g, float b, float alpha = 1.f );
		void setVolumeRenderingMode( VolumeRenderingMode renderingMode );

		// 		void clearGrids() { m_grids.clear(); updateGrids(); }
		// 		void setGrid( openvdb::FloatGrid::Ptr grid, std::string identifier ) { m_grids[identifier] = grid; updateGrids(); }
		inline void setGrid( openvdb::GridBase::Ptr grid ) { m_grid = grid; updateGrids();  }

		nvdb::Scene* gvdbScene() { return m_gvdb.getScene(); }

		virtual void resizeEvent( QResizeEvent* e ) override;
		inline VolumeRenderingMode volumeRenderingMode() const { return m_volumeRenderingMode; }

	protected:
		virtual void draw();
		virtual void init();
		virtual QString helpString() const;
		void updateGrids();
		void updateRenderingMode();

		nvdb::VolumeGVDB m_gvdb;
		GLuint m_renderTexture = -1;
		openvdb::GridBase::Ptr m_grid = nullptr;
		VolumeRenderingMode m_volumeRenderingMode = VolumeRenderingMode::Transparent;
		// 		std::map<std::string, openvdb::GridBase::Ptr> m_grids;
};
