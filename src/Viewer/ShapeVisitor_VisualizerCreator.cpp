#include "Viewer/ShapeVisitor_VisualizerCreator.h"
//-----------------------------------------------------------------------------
#include "Util/ApplicationConfig.h"
//-----------------------------------------------------------------------------
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
#include <osg/BlendFunc>
//-----------------------------------------------------------------------------

namespace Vwr {

osg::Node * ShapeVisitor_VisualizerCreator::getCreatedVisualizer (void) {
	return createdVisualizer_;
}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Null & shape) {
	createdVisualizer_ = new osg::Group;
}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Composite & shape) {
	osg::Group * group = new osg::Group;
	Layout::Shape_Composite::ShapesListType & shapes = shape.getShapes ();
	for (Layout::Shape_Composite::ShapesListType::iterator it = shapes.begin (); it != shapes.end (); ++it) {
		(*it)->accept (*this);
		group->addChild (createdVisualizer_);
	}

	createdVisualizer_ = group;
}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Plane & shape) {
	// OSG does not support InfinitePlane drawing at this time, maybe try to use this piece of code with the new version of OSG
	/*
	osg::InfinitePlane * plane = new osg::InfinitePlane;
	plane->set (shape.getNormalVector (), getScaledDistance (shape.getD ()));

	osg::ShapeDrawable * sd = new osg::ShapeDrawable;
	sd->setShape (plane);
	sd->setColor (osg::Vec4 (0, 0.5, 0.0, 0.5));
	*/

	// now we use a temporary code instead - drawing a plane using box:

	// center
	osg::Vec3 center (0, 0, (- shape.getD ()) / shape.getNormalVector ().z () ); // some point on the plane

	// rotation
	osg::Quat quat;
	quat.makeRotate (osg::Z_AXIS, shape.getNormalVector ());

	osg::Box * box = new osg::Box;
	box->setCenter (getScaledPosition (center));
	box->setRotation (quat);
	box->setHalfLengths (osg::Vec3 (1000, 1000, 1));

	osg::ShapeDrawable * sd = new osg::ShapeDrawable;
	sd->setShape (box);
	sd->setColor (osg::Vec4 (0, 0, 1.0, 0.06));
	sd->getOrCreateStateSet()->setMode (GL_BLEND, osg::StateAttribute::ON);
	sd->getStateSet()->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);

	osg::Geode * geode = new osg::Geode;
	geode->addDrawable (sd);

	createdVisualizer_ = geode;
}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Sphere & shape) {
	visualizeSphere (shape);
}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_SphereSurface & shape) {
	visualizeSphere (shape);
}

void ShapeVisitor_VisualizerCreator::visualizeSphere (Layout::Shape_AbstractSphere & abstractSphere) {
	osg::Sphere * sphere = new osg::Sphere;
	sphere->setRadius (getScaledDistance (abstractSphere.getRadius ()));
	sphere->setCenter (getScaledPosition (abstractSphere.getCenter ()));

	osg::ShapeDrawable * sd = new osg::ShapeDrawable;
	sd->setShape (sphere);
	sd->setColor (osg::Vec4 (0, 0, 1.0, 0.06));

	//transparency of sphere
	sd->getOrCreateStateSet()->setMode (GL_BLEND, osg::StateAttribute::ON);
	sd->getStateSet()->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);
	sd->getStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	sd->getStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	sd->getStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
	sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	sd->getStateSet()->setRenderBinDetails( 11, "RenderBin");
	//

	osg::Geode * geode = new osg::Geode;
	geode->addDrawable (sd);

	createdVisualizer_ = geode;
}

float ShapeVisitor_VisualizerCreator::getScaledDistance (
	const float & distance
) {
	return distance * getScale ();
}

osg::Vec3f ShapeVisitor_VisualizerCreator::getScaledPosition (
	const osg::Vec3f & position
) {
	return position * getScale ();
}

float ShapeVisitor_VisualizerCreator::getScale () {
	return Util::ApplicationConfig::get()->getValue("Viewer.Display.NodeDistanceScale").toFloat();
}

} // namespace
