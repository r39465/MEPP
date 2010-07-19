///////////////////////////////////////////////////////////////////////////
// Author: Martial TOLA
// Year: 2010
// CNRS-Lyon, LIRIS UMR 5205
/////////////////////////////////////////////////////////////////////////// 
#include "viewer.hxx"
#include "scene.h"

#include "mepp_component_plugin_interface.h"

using namespace qglviewer;

Viewer::Viewer(QWidget *parent, QList<mepp_component_plugin_interface *> lp) : QGLViewer(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	m_parent = parent;
	lplugin = lp;

	// rendering options
	m_Lighting = true;
	m_Culling = false;
	m_FirstView = true;
	m_UseNormals = true;
	m_Antialiasing = false;
	m_SmoothShading = true;
	m_UseVertexColor = false;
	m_UseFaceColor = false;
	m_PolygonMode = GL_FILL;
	m_SuperimposeEdges = true;
	m_DrawVoronoiEdges = false;
	m_DrawBoundingBox = false;
	m_SuperimposeVertices = false;
	m_ThicknessControlEdges = 3.0f;
	m_DrawBoundingBoxWhenMoving = false;

	// other options
	m_PointSize = 3.0f;
	m_BackColor[0] = m_BackColor[1] = m_BackColor[2] = 0.4f;
	m_MeshColor[0] = m_MeshColor[1] = m_MeshColor[2] = 1.0f;
	m_EdgeColor[0] = m_EdgeColor[1] = m_EdgeColor[2] = 0.0f;
	m_VertexColor[0] = m_VertexColor[1] = m_VertexColor[2] = 0.0f;

	// mouse
	m_LeftButtonDown = false;
	m_RightButtonDown = false;
	m_Moving = false;

	mCouplingTranslations = mCouplingRotations = mCouplingZooms = false;
	
	show_normals = false;
	VBO_mode = false;
	save_animation = false;

	timerDynamic = new QTimer(this);
    connect(timerDynamic, SIGNAL(timeout()), this, SLOT(shotDynamic()));
	m_fps = 24;	//12;
	m_reverse = m_loop = false;
}

Viewer::~Viewer()
{
	frame_.clear();

	delete timerDynamic;
}

void Viewer::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void Viewer::setScenePtr(ScenePtr scene_ptr)
{
	this->scene_ptr = scene_ptr;
}

// change material
void Viewer::change_material(string mat_name)
{
	float ambient[]  = {0.0f,0.0f,0.0f,1.0f};
	float diffuse[]  = {0.0f,0.0f,0.0f,1.0f};
	float specular[]  = {0.0f,0.0f,0.0f,1.0f};
	float emission[]  = {0.3f,0.3f,0.3f,1.0f};
	float shininess[] = {0.0f};

	// Change
	if (mat_name == "Silver")
	{
		// Ambient
		ambient[0] = 0.19225f;
		ambient[1] = 0.19225f;
		ambient[2] = 0.19225f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.50754f;
		diffuse[1] = 0.50754f;
		diffuse[2] = 0.50754f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.508273f;
		specular[1] = 0.508273f;
		specular[2] = 0.508273f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 51.2f;
	}
	else if (mat_name == "Gold")
	{
		// Ambient
		ambient[0] = 0.24725f;
		ambient[1] = 0.1995f;
		ambient[2] = 0.0745f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.75164f;
		diffuse[1] = 0.60648f;
		diffuse[2] = 0.22648f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.928281f;
		specular[1] = 0.855802f;
		specular[2] = 0.666065f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 51.2f;
	}
	else if (mat_name == "Jade")
	{
		// Ambient
		ambient[0] = 0.135f;
		ambient[1] = 0.2225f;
		ambient[2] = 0.1575f;
		ambient[3] = 0.95f;
		// Diffuse
		diffuse[0] = 0.54f;
		diffuse[1] = 0.89f;
		diffuse[2] = 0.63f;
		diffuse[3] = 0.95f;
		// Specular
		specular[0] = 0.316228f;
		specular[1] = 0.316228f;
		specular[2] = 0.316228f;
		specular[3] = 0.95f;
		// Shininess
		shininess[0] = 12.8f;
	}
	else if (mat_name == "Light blue")
	{
		// Ambient
		ambient[0] = 0.0f;
		ambient[1] = 0.5f;
		ambient[2] = 0.75f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.0f;
		diffuse[1] = 0.5f;
		diffuse[2] = 1.0f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.75f;
		specular[1] = 0.75f;
		specular[2] = 0.75f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 64.0f;
	}
	else if (mat_name == "Emerald")
	{
		// Ambient
		ambient[0] = 0.0215f;
		ambient[1] = 0.1745f;
		ambient[2] = 0.0215f;
		ambient[3] = 0.55f;
		// Diffuse
		diffuse[0] = 0.07568f;
		diffuse[1] = 0.61424f;
		diffuse[2] = 0.07568f;
		diffuse[3] = 0.55f;
		// Specular
		specular[0] = 0.633f;
		specular[1] = 0.727811f;
		specular[2] = 0.633f;
		specular[3] = 0.55f;
		// Shininess
		shininess[0] = 76.8f;
	}
	else if (mat_name == "Polished silver")
	{
		// Ambient
		ambient[0] = 0.23125f;
		ambient[1] = 0.23125f;
		ambient[2] = 0.23125f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.2775f;
		diffuse[1] = 0.2775f;
		diffuse[2] = 0.2775f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.773911f;
		specular[1] = 0.773911f;
		specular[2] = 0.773911f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 89.6f;
	}
	else if (mat_name == "Chrome")
	{
		// Ambient
		ambient[0] = 0.25f;
		ambient[1] = 0.25f;
		ambient[2] = 0.25f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.4f;
		diffuse[1] = 0.4f;
		diffuse[2] = 0.4f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.774597f;
		specular[1] = 0.774597f;
		specular[2] = 0.774597f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 76.8f;
	}
	else if (mat_name == "Copper")
	{
		// Ambient
		ambient[0] = 0.19125f;
		ambient[1] = 0.0735f;
		ambient[2] = 0.0225f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.7038f;
		diffuse[1] = 0.27048f;
		diffuse[2] = 0.0828f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.256777f;
		specular[1] = 0.137622f;
		specular[2] = 0.086014f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 12.8f;
	}
	else if (mat_name == "Polished gold")
	{
		// Ambient
		ambient[0] = 0.24725f;
		ambient[1] = 0.2245f;
		ambient[2] = 0.0645f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.34615f;
		diffuse[1] = 0.3143f;
		diffuse[2] = 0.0903f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.797357f;
		specular[1] = 0.723991f;
		specular[2] = 0.208006f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 83.2f;
	}
	else if (mat_name == "Pewter")
	{
		// Ambient
		ambient[0] = 0.105882f;
		ambient[1] = 0.058824f;
		ambient[2] = 0.113725f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.427451f;
		diffuse[1] = 0.470588f;
		diffuse[2] = 0.541176f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.333333f;
		specular[1] = 0.333333f;
		specular[2] = 0.521569f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 9.84615f;
	}
	else if (mat_name == "Obsidian")
	{
		// Ambient
		ambient[0] = 0.05375f;
		ambient[1] = 0.05f;
		ambient[2] = 0.06625f;
		ambient[3] = 0.82f;
		// Diffuse
		diffuse[0] = 0.18275f;
		diffuse[1] = 0.17f;
		diffuse[2] = 0.22525f;
		diffuse[3] = 0.82f;
		// Specular
		specular[0] = 0.332741f;
		specular[1] = 0.328634f;
		specular[2] = 0.346435f;
		specular[3] = 0.82f;
		// Shininess
		shininess[0] = 38.4f;
	}
	else if (mat_name == "Black plastic")
	{
		// Ambient
		ambient[0] = 0.0f;
		ambient[1] = 0.0f;
		ambient[2] = 0.0f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.01f;
		diffuse[1] = 0.01f;
		diffuse[2] = 0.01f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.5f;
		specular[1] = 0.5f;
		specular[2] = 0.5f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 32.0f;
	}
	else if (mat_name == "Polished bronze")
	{
		// Ambient
		ambient[0] = 0.25f;
		ambient[1] = 0.148f;
		ambient[2] = 0.006475f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.4f;
		diffuse[1] = 0.2368f;
		diffuse[2] = 0.1036f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.774597f;
		specular[1] = 0.458561f;
		specular[2] = 0.200621f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 76.8f;
	}
	else if (mat_name == "Polished copper")
	{
		// Ambient
		ambient[0] = 0.2295f;
		ambient[1] = 0.08825f;
		ambient[2] = 0.0275f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.5508f;
		diffuse[1] = 0.2118f;
		diffuse[2] = 0.066f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.580594f;
		specular[1] = 0.223257f;
		specular[2] = 0.0695701f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 51.2f;
	}
	else if (mat_name == "Pearl")
	{
		// Ambient
		ambient[0] = 0.25f;
		ambient[1] = 0.20725f;
		ambient[2] = 0.20725f;
		ambient[3] = 0.922f;
		// Diffuse
		diffuse[0] = 1.0f;
		diffuse[1] = 0.829f;
		diffuse[2] = 0.829f;
		diffuse[3] = 0.922f;
		// Specular
		specular[0] = 0.296648f;
		specular[1] = 0.296648f;
		specular[2] = 0.296648f;
		specular[3] = 0.922f;
		// Shininess
		shininess[0] = 11.264f;
	}
	else if (mat_name == "Ruby")
	{
		// Ambient
		ambient[0] = 0.1745f;
		ambient[1] = 0.01175f;
		ambient[2] = 0.01175f;
		ambient[3] = 0.55f;
		// Diffuse
		diffuse[0] = 0.61424f;
		diffuse[1] = 0.04136f;
		diffuse[2] = 0.04136f;
		diffuse[3] = 0.55f;
		// Specular
		specular[0] = 0.727811f;
		specular[1] = 0.626959f;
		specular[2] = 0.626959f;
		specular[3] = 0.55f;
		// Shininess
		shininess[0] = 76.8f;
	}
	else if (mat_name == "Turquoise")
	{
		// Ambient
		ambient[0] = 0.1f;
		ambient[1] = 0.18725f;
		ambient[2] = 0.1745f;
		ambient[3] = 0.8f;
		// Diffuse
		diffuse[0] = 0.396f;
		diffuse[1] = 0.74151f;
		diffuse[2] = 0.69102f;
		diffuse[3] = 0.8f;
		// Specular
		specular[0] = 0.297254f;
		specular[1] = 0.30829f;
		specular[2] = 0.306678f;
		specular[3] = 0.8f;
		// Shininess
		shininess[0] = 12.8f;
	}
	else if (mat_name == "Brass")
	{
		// Ambient
		ambient[0] = 0.329412f;
		ambient[1] = 0.223529f;
		ambient[2] = 0.027451f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.780392f;
		diffuse[1] = 0.268627f;
		diffuse[2] = 0.113725f;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0.992157f;
		specular[1] = 0.741176f;
		specular[2] = 0.807843f;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 27.8974f;
	}
	else if (mat_name == " None ")
	{
        // Ambient
        ambient[0] = 0.3f;
		ambient[1] = 0.3f;
		ambient[2] = 0.3f;
		ambient[3] = 1.0f;
		// Diffuse
		diffuse[0] = 0.5;
		diffuse[1] = 0.5;
		diffuse[2] = 0.5;
		diffuse[3] = 1.0f;
		// Specular
		specular[0] = 0;
		specular[1] = 0;
		specular[2] = 0;
		specular[3] = 1.0f;
		// Shininess
		shininess[0] = 27.8974f;
	}

	// apply
	glMaterialfv( GL_FRONT, GL_AMBIENT,   ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE,   diffuse);
	glMaterialfv( GL_FRONT, GL_SPECULAR,  specular);
	glMaterialfv( GL_FRONT, GL_SHININESS, shininess);
	glMaterialfv( GL_FRONT, GL_EMISSION,  emission);

	m_last_material = mat_name;
}

void Viewer::render()
{
	// shading option
	if (m_SmoothShading)
	{
		glShadeModel(GL_SMOOTH);
	}
	else
	{
		glShadeModel(GL_FLAT);
	}

	// culling option
	if (m_Culling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	// polygon mode (point, line or fill)
	glPolygonMode(GL_FRONT_AND_BACK,m_PolygonMode);

	if ( m_UseVertexColor || m_UseFaceColor )
	{
		if ( !glIsEnabled(GL_COLOR_MATERIAL) )
		{
			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
			glEnable(GL_COLOR_MATERIAL);
		}
	}
	else
	{
		if ( glIsEnabled(GL_COLOR_MATERIAL) )
		{
			glDisable(GL_COLOR_MATERIAL);
			change_material(m_last_material);
		}

		// set mesh color
		glColor3f(m_MeshColor[0],m_MeshColor[1],m_MeshColor[2]);
	}

	// antialiasing
	if (m_Antialiasing)
	{
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		glLineWidth(1.5f);
	}
	else
	{
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_BLEND);
		glLineWidth(1.0f);
	}

	// drawing
	glPushMatrix();

	// User's pre_draw - Matrices protected - Attributes NOT protected
	if (((mainwindow *)getParent())->activeMdiChild() == this)
	{
		glPushMatrix();
			for (int p=0; p<lplugin.size(); p++)
				lplugin[p]->pre_draw();
		glPopMatrix();
	}

	if (m_SuperimposeEdges || m_SuperimposeVertices)
	{
		// enable polygon offset
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(3.0f,1.0f);
	}

	// draw the mesh
	if ((m_Moving && m_DrawBoundingBoxWhenMoving) ||
			m_DrawBoundingBox)
	{
		glColor3f(1.0f,0.0f,0.0f);
		glDisable(GL_LIGHTING);
		scene_ptr->get_polyhedron()->gl_draw_bounding_box();
		glColor3f(m_MeshColor[0],m_MeshColor[1],m_MeshColor[2]); // new
	}

	// lighting option
	if (m_Lighting)
	{
		m_UseNormals = true;
		glEnable(GL_LIGHTING);
	}
	else
	{
		m_UseNormals = false;
		glDisable(GL_LIGHTING);
	}

	if (!m_Moving || !m_DrawBoundingBoxWhenMoving)
	{
		scene_ptr->get_polyhedron()->gl_draw(m_SmoothShading, m_UseNormals, m_UseVertexColor, m_UseFaceColor);
	}

	// disable lighting
	if (m_SuperimposeEdges || m_SuperimposeVertices)
	{
        glDisable(GL_LIGHTING);
	}

	// draw the mesh once again with a few options desactivated
	if (m_SuperimposeEdges && !(m_Moving && m_DrawBoundingBoxWhenMoving))
	{
		// set line mode
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		// edge color
		glColor3f(m_EdgeColor[0],m_EdgeColor[1],m_EdgeColor[2]);

		// superimpose edges on the mesh
		scene_ptr->get_polyhedron()->superimpose_edges(m_DrawVoronoiEdges);
	}
	// end superimpose edges

	// superimpose vertices
	if (m_SuperimposeVertices && !(m_Moving && m_DrawBoundingBoxWhenMoving))
	{
		glColor3f(m_VertexColor[0],m_VertexColor[1],m_VertexColor[2]);
		scene_ptr->get_polyhedron()->superimpose_spheres(0.1);
	}
	// end superimpose vertices

	// disable polygon offset
	if (m_SuperimposeEdges || m_SuperimposeVertices)
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	// go back to previous rendering mode (line, edge, fill)
	if (m_SuperimposeEdges && !(m_Moving && m_DrawBoundingBoxWhenMoving))
	{
		glPolygonMode(GL_FRONT_AND_BACK,m_PolygonMode);
		if (m_Lighting)
		{
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
	}

	// User's post_draw - Matrices protected - Attributes NOT protected
	if (((mainwindow *)getParent())->activeMdiChild() == this)
	{
		glPushMatrix();
			for (int p=0; p<lplugin.size(); p++)
				lplugin[p]->post_draw();
		glPopMatrix();
	}

	glPopMatrix();

	// Flush
    glFlush();
}

void Viewer::init()
{
	// Swap the CAMERA and FRAME state keys (NoButton and Control)
	// Save CAMERA binding first. See setHandlerKeyboardModifiers() documentation.
	/*#if QT_VERSION < 0x040000
		setHandlerKeyboardModifiers(QGLViewer::CAMERA, Qt::AltButton);
		setHandlerKeyboardModifiers(QGLViewer::FRAME,  Qt::NoButton);
		setHandlerKeyboardModifiers(QGLViewer::CAMERA, Qt::ControlButton);
	#else
		setHandlerKeyboardModifiers(QGLViewer::CAMERA, Qt::AltModifier);
		setHandlerKeyboardModifiers(QGLViewer::FRAME,  Qt::NoModifier);
		setHandlerKeyboardModifiers(QGLViewer::CAMERA, Qt::ControlModifier);
	#endif*/

	#ifdef GL_RESCALE_NORMAL  // OpenGL 1.2 Only...
		glEnable(GL_RESCALE_NORMAL);
	#endif

	// do not save the state of the viewer
	setStateFileName(QString::null);

	// Make world axis visible
	//setAxisIsDrawn(); // avoid bug under Mac OS X

	camera()->setZClippingCoefficient(100.0);	// 50

	showAllScene();

	initialCameraPosition = camera()->position();
	initialCameraOrientation = camera()->orientation();

	// Enable semi-transparent culling planes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	setBackgroundColor(QColor (220, 220, 220));

	// back material
	change_material("Light blue");
}

void Viewer::postSelection(const QPoint&)
{
	if (scene_ptr->get_loadType()!=Space)
		return;

	if (selectedName() == -1)
	{
		setManipulatedFrame(frame(0));
		setSelectedFrameNumber(0);

		scene_ptr->set_current_polyhedron(0);
	}
	else
	{
		setManipulatedFrame(frame(selectedName()));
		setSelectedFrameNumber(selectedName());

		scene_ptr->set_current_polyhedron(selectedName());
	}

	this->setWindowTitle(tr("%1 - (Space: %2/%3)")
									.arg(userFriendlyCurrentFile())
									.arg(scene_ptr->get_current_polyhedron()+1)
									.arg(scene_ptr->get_nb_polyhedrons()));
}

void Viewer::drawWithNames()
{
	// render scene with objects ids
	if (scene_ptr->get_loadType()==Space)
		dessine_space(true);
}

void Viewer::draw()
{
	if (scene_ptr->get_loadType()==Space)
		dessine_space();
	else
		dessine();

	((mainwindow *)m_parent)->update_mesh_properties(false, false);
}

void Viewer::dessine_space(bool names)
{
	// Here we are in the world coordinate system. Draw your scene here.

	// Scale down the drawings
	//glScalef(0.3f, 0.3f, 0.3f);

	int save_polyhedron = scene_ptr->get_current_polyhedron();
	Quaternion q = frame(save_polyhedron)->orientation();

	// -----------------------

	if (!names)
	{
		if (((mainwindow *)getParent())->activeMdiChild() == this)
		{
			glPushMatrix();
				for (int p=0; p<lplugin.size(); p++)
					lplugin[p]->pre_draw_all_scene();
			glPopMatrix();
		}
	}

	// -----------------------

	glClearColor(m_BackColor[0], m_BackColor[1], m_BackColor[2], 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int nbMesh = qMin(scene_ptr->get_nb_polyhedrons(), get_nb_frames());
	for (int i=0; i<nbMesh; i++)
	{
		if (mCouplingRotations)
			frame(i)->setOrientation(q);

		glPushMatrix(); // Save the current model view matrix
		if (names) glPushName(i);

			glMultMatrixd(frame(i)->matrix()); // Multiply matrix to get in the frame coordinate system
			
			scene_ptr->set_current_polyhedron(i);
			render(); // Draws the scene

		if (names) glPopName();
		glPopMatrix(); // Restore the original (world) coordinate system
	}

	scene_ptr->set_current_polyhedron(save_polyhedron);

	// -----------------------

	if (!names)
	{
		if (((mainwindow *)getParent())->activeMdiChild() == this)
		{
			glPushMatrix();
				for (int p=0; p<lplugin.size(); p++)
					lplugin[p]->post_draw_all_scene();
			glPopMatrix();
		}
	}
}

void Viewer::dessine(bool names)
{
	// Here we are in the world coordinate system. Draw your scene here.

	// Scale down the drawings
	//glScalef(0.3f, 0.3f, 0.3f);

	if (!names)
	{
		glClearColor(m_BackColor[0], m_BackColor[1], m_BackColor[2], 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render(); // Draws the scene
	}
}

void Viewer::shotDynamic()
{
	if (!m_reverse)
	{
		if (scene_ptr->get_current_polyhedron() < (scene_ptr->get_nb_polyhedrons()-1))
			scene_ptr->set_current_polyhedron(scene_ptr->get_current_polyhedron()+1);
		else
		{
			if (m_loop)
				scene_ptr->set_current_polyhedron(0);
			else
			{
				timerDynamic->stop();
				return;
			}
		}
	}
	else
	{
		if (scene_ptr->get_current_polyhedron() > 0)
			scene_ptr->set_current_polyhedron(scene_ptr->get_current_polyhedron()-1);
		else
		{
			if (m_loop)
				scene_ptr->set_current_polyhedron((scene_ptr->get_nb_polyhedrons()-1));
			else
			{
				timerDynamic->stop();
				return;
			}
		}
	}

	setDynTitle();
	updateGL();
}

void Viewer::shotCapture()
{
}

void Viewer::mousePressEvent(QMouseEvent *event)
{
	QGLViewer::mousePressEvent(event);

	if (((mainwindow *)getParent())->activeMdiChild() == this)
	{
#ifdef __linux__
		if (QApplication::keyboardModifiers() == Qt::MetaModifier)
#else
		if (QApplication::keyboardModifiers() == Qt::AltModifier)
#endif
		{
			if (event->button() == Qt::LeftButton)
			{
				for (int p=0; p<lplugin.size(); p++)
					lplugin[p]->OnMouseLeftDown(event);
			}
			else if (event->button() == Qt::RightButton)
			{
				for (int p=0; p<lplugin.size(); p++)
					lplugin[p]->OnMouseRightDown(event);
			}
		}
	}
}

void Viewer::mouseMoveEvent(QMouseEvent *event)
{
	QGLViewer::mouseMoveEvent(event);
	m_Moving = true;

	if (((mainwindow *)getParent())->activeMdiChild() == this)
	{
#ifdef __linux__
		if (QApplication::keyboardModifiers() == Qt::MetaModifier)
#else
		if (QApplication::keyboardModifiers() == Qt::AltModifier)
#endif
		{
			for (int p=0; p<lplugin.size(); p++)
				lplugin[p]->OnMouseMotion(event);
		}
	}
}

void Viewer::mouseReleaseEvent(QMouseEvent *event)
{
	QGLViewer::mouseReleaseEvent(event);
	m_Moving = false;
	updateGL();

	if (((mainwindow *)getParent())->activeMdiChild() == this)
	{
#ifdef __linux__
		if (QApplication::keyboardModifiers() == Qt::MetaModifier)
#else
		if (QApplication::keyboardModifiers() == Qt::AltModifier)
#endif
		{
			if (event->button() == Qt::LeftButton)
			{
				for (int p=0; p<lplugin.size(); p++)
					lplugin[p]->OnMouseLeftUp(event);
			}
			else if (event->button() == Qt::RightButton)
			{
				for (int p=0; p<lplugin.size(); p++)
					lplugin[p]->OnMouseRightUp(event);
			}
		}
	}
}

void Viewer::wheelEvent(QWheelEvent *event)
{
	QGLViewer::wheelEvent(event);

	if (((mainwindow *)getParent())->activeMdiChild() == this)
	{
#ifdef __linux__
		if (QApplication::keyboardModifiers() == Qt::MetaModifier)
#else
		if (QApplication::keyboardModifiers() == Qt::AltModifier)
#endif
		{
			for (int p=0; p<lplugin.size(); p++)
				lplugin[p]->OnMouseWheel(event);
		}
	}
}

void Viewer::keyPressEvent(QKeyEvent *event)
{
	QGLViewer::keyPressEvent(event);

	if (((mainwindow *)getParent())->activeMdiChild() == this)
	{
#ifdef __linux__
		if (QApplication::keyboardModifiers() == Qt::MetaModifier)
#else
		if (QApplication::keyboardModifiers() == Qt::AltModifier)
#endif
		{
			for (int p=0; p<lplugin.size(); p++)
				lplugin[p]->OnKeyPress(event);
		}
	}
}

void Viewer::keyReleaseEvent(QKeyEvent *event)
{
	QGLViewer::keyReleaseEvent(event);

	if (((mainwindow *)getParent())->activeMdiChild() == this)
	{
#ifdef __linux__
		if (QApplication::keyboardModifiers() == Qt::MetaModifier)
#else
		if (QApplication::keyboardModifiers() == Qt::AltModifier)
#endif
		{
			for (int p=0; p<lplugin.size(); p++)
				lplugin[p]->OnKeyRelease(event);
		}
	}
}

QString Viewer::helpString() const
{
	QString text(tr("<h2>MEPP</h2>"));

	text += tr("HELP: todo");

	return text;
}