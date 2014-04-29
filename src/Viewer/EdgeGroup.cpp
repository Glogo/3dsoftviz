#include "Viewer/EdgeGroup.h"

#include "Viewer/TextureWrapper.h"
#include "Data/Node.h"

#include <osg/Depth>
#include <osg/BlendFunc>
#include <osg/CullFace>

using namespace Vwr;

EdgeGroup::EdgeGroup(QMap<qlonglong, osg::ref_ptr<Data::Edge> > *edges, float scale)
{
	this->edges = edges;
	this->scale = scale;

	createEdgeStateSets();
	initEdges();
}

EdgeGroup::~EdgeGroup(void)
{
}

/*!
 *
 * \param scale
 * Koeficient velkosti hran.
 *
 * \returns
 * Podgraf hran.
 *
 *
 * Vytvori podgraf s hranami, ktore spajaju uzly na zaklade ich aktualnej pozicie.
 *
 */
void EdgeGroup::initEdges()
{
    /*
    cout << "initEdges" << std::endl;
	osg::ref_ptr<osg::Group> allEdges = new osg::Group;

	osg::ref_ptr<osg::Vec2Array> edgeTexCoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec4Array> orientedEdgeColors = new osg::Vec4Array;

	geometry = new osg::Geometry;
	orientedGeometry = new osg::Geometry;

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = edges->begin();

	int edgePos = 0;

	while (i != edges->end())
	{
		getEdgeCoordinatesAndColors(i.value(), edgePos, coordinates, edgeTexCoords, colors, orientedEdgeColors);
		edgePos += 4;

		if (i.value()->isOriented())
			orientedGeometry->addPrimitiveSet(i.value());
		else
			geometry->addPrimitiveSet(i.value());

		i++;
	}

	geometry->setTexCoordArray(0, edgeTexCoords);
	geometry->setVertexArray(coordinates);
	geometry->setColorArray(colors);
#ifdef BIND_PER_PRIMITIVE
	geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
#else
	geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
#endif
	geometry->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	//geometry->getStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);

	orientedGeometry->setTexCoordArray(0, edgeTexCoords);
	orientedGeometry->setVertexArray(coordinates);
	orientedGeometry->setColorArray(orientedEdgeColors);
#ifdef BIND_PER_PRIMITIVE
	orientedGeometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
#else
	orientedGeometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
#endif
	orientedGeometry->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	//orientedGeometry->getStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);

	osg::ref_ptr<osg::Geode> g1 = new osg::Geode;

	g1->addDrawable(geometry);
	g1->setStateSet(edgeStateSet);

	osg::ref_ptr<osg::Geode> g2 = new osg::Geode;
	g2->addDrawable(orientedGeometry);
	g2->setStateSet(orientedEdgeStateSet);

	allEdges->addChild(g1);
	allEdges->addChild(g2);

    */
    this->geodes = new QMap<osg::ref_ptr<Data::Edge>, osg::ref_ptr<osg::Geode> >();
    this->edgeGroup = new osg::Group();
}

void EdgeGroup::updateEdgeCoords()
{
    /*
    cout << "updateEdgeCoords" << std::endl;
	osg::ref_ptr<osg::Vec2Array> edgeTexCoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec4Array> orientedEdgeColors = new osg::Vec4Array;

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = edges->begin();

	int edgePos = 0;

	while (i != edges->end())
	{
		getEdgeCoordinatesAndColors(i.value(), edgePos, coordinates, edgeTexCoords, colors, orientedEdgeColors);
		edgePos += 4;
		i++;
	}

	geometry->setVertexArray(coordinates);
	geometry->setTexCoordArray(0, edgeTexCoords);
	geometry->setColorArray(colors);

	orientedGeometry->setVertexArray(coordinates);
	orientedGeometry->setTexCoordArray(0, edgeTexCoords);
	orientedGeometry->setColorArray(orientedEdgeColors);
    */

    for (QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = edges->begin(); i != edges->end(); i++){
        osg::ref_ptr<Data::Edge> edge = *i;

        osg::Vec3 srcNodePosition = edge->getSrcNode()->getCurrentPosition();
        osg::Vec3 dstNodePosition = edge->getDstNode()->getCurrentPosition();

        edge->updateCoordinates(srcNodePosition, dstNodePosition);
        if (!geodes->contains(edge)) {
            osg::ref_ptr<osg::Geode> geode = new osg::Geode();
            osg::ref_ptr<osg::Geometry> edgeQuad = new osg::Geometry;

            edgeQuad->setVertexArray(edge->getCooridnates());

            edgeQuad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

            edgeQuad->setTexCoordArray(0, edge->getEdgeTexCoords());

            osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
            colorArray->push_back(edge->getEdgeColor());
            edgeQuad->setColorArray( colorArray);

            edgeQuad->setColorBinding(osg::Geometry::BIND_OVERALL);
            if (edge->isOriented())
                edgeQuad->setStateSet(orientedEdgeStateSet);
            else
                edgeQuad->setStateSet(edgeStateSet);

            geode->addDrawable(edgeQuad);

            geodes->insert(edge, geode);
            edgeGroup->addChild(geode);
        } else {
            osg::ref_ptr<osg::Geometry> edgeQuad = dynamic_cast<osg::Geometry *>(geodes->value(edge)->getDrawable(0));

            edgeQuad->setVertexArray(edge->getCooridnates());

            edgeQuad->setTexCoordArray(0, edge->getEdgeTexCoords());

            osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
            colorArray->push_back(edge->getEdgeColor());
            edgeQuad->setColorArray( colorArray);
        }
    }
}

void EdgeGroup::getEdgeCoordinatesAndColors(osg::ref_ptr<Data::Edge> edge, int first,
											osg::ref_ptr<osg::Vec3Array> coordinates,
											osg::ref_ptr<osg::Vec2Array> edgeTexCoords,
											osg::ref_ptr<osg::Vec4Array> colors,
											osg::ref_ptr<osg::Vec4Array> orientedEdgeColors)
{
	osg::Vec3 srcNodePosition = edge->getSrcNode()->getCurrentPosition();
	osg::Vec3 dstNodePosition = edge->getDstNode()->getCurrentPosition();

	edge->updateCoordinates(srcNodePosition, dstNodePosition);
	edge->setFirst(first);

	coordinates->push_back(edge->getCooridnates()->at(0));
	coordinates->push_back(edge->getCooridnates()->at(1));
	coordinates->push_back(edge->getCooridnates()->at(2));
	coordinates->push_back(edge->getCooridnates()->at(3));

	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(0));
	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(1));
	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(2));
	edgeTexCoords->push_back(edge->getEdgeTexCoords()->at(3));

	if (edge->isOriented())
		orientedEdgeColors->push_back(edge->getEdgeColor());
	else
		colors->push_back(edge->getEdgeColor());
}

void EdgeGroup::synchronizeEdges()
{
//	QList<qlonglong> edgeKeys = edges->keys();

//	for (int i = 0; i < 2; i++)
//	{
//		osg::ref_ptr<osg::Geometry> geometry = edgeGroup->getChild(i)->asGeode()->getDrawable(0)->asGeometry();
//		const osg::Geometry::PrimitiveSetList primitives = geometry->getPrimitiveSetList();

//		for (unsigned int x = 0; x < primitives.size() ; x++)
//		{
//			Data::Edge * e = dynamic_cast<Data::Edge * >(primitives.at(x).get());

//			if (!edgeKeys.contains(e->getId()))
//			{
//				geometry->removePrimitiveSet(geometry->getPrimitiveSetIndex((e)));
//			}
//		}
//	}

//	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator ie = edges->begin();

//	while (ie != edges->end())
//	{
//		if (!(*ie)->isOriented() && geometry->getPrimitiveSetIndex((*ie)) == geometry->getNumPrimitiveSets())
//			geometry->addPrimitiveSet(*ie);
//		else if ((*ie)->isOriented() && orientedGeometry->getPrimitiveSetIndex((*ie)) == orientedGeometry->getNumPrimitiveSets())
//			orientedGeometry->addPrimitiveSet(*ie);

//		ie++;
//	}
}

void EdgeGroup::createEdgeStateSets()
{
	edgeStateSet = new osg::StateSet;

    edgeStateSet->setDataVariance(osg::Object::DYNAMIC);
    edgeStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    edgeStateSet->setTextureAttributeAndModes(0, TextureWrapper::getEdgeTexture(), osg::StateAttribute::ON);

    edgeStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    edgeStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

    edgeStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    osg::ref_ptr<osg::Depth> depth = new osg::Depth;
    depth->setWriteMask(false);
    edgeStateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

    osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
    cull->setMode(osg::CullFace::BACK);
    edgeStateSet->setAttributeAndModes(cull, osg::StateAttribute::ON);






	orientedEdgeStateSet = new osg::StateSet;

    orientedEdgeStateSet->setDataVariance(osg::Object::DYNAMIC);
	orientedEdgeStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	orientedEdgeStateSet->setTextureAttributeAndModes(0, TextureWrapper::getOrientedEdgeTexture(), osg::StateAttribute::ON);
	orientedEdgeStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	orientedEdgeStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	orientedEdgeStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	orientedEdgeStateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);
}
