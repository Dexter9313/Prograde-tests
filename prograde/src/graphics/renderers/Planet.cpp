/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "graphics/renderers/Planet.hpp"

unsigned int& Planet::cubemapsSize()
{
	static unsigned int cubemapsSize = 2048;
	return cubemapsSize;
}

Planet::Planet(float radius, QVector3D const& oblateness)
    : radius(radius)
    , oblateness(oblateness)
{
	cubemapDiffuse = GLHandler::newRenderTarget(cubemapsSize(), cubemapsSize(),
	                                            GL_RGBA16F, true);
}

void Planet::initTerrestrial(QColor const& color, float polarLatitude,
                             float atmosphere)
{
	valid  = true;
	shader = GLHandler::newShader("planet/planet", "planet/diffnormplanet");
	mesh   = Primitives::newUnitSphere(shader, 50, 50);
	normal = true;
	cubemapNormal = GLHandler::newRenderTarget(cubemapsSize(), cubemapsSize(),
	                                           GL_RGBA16F, true);

	GLHandler::setShaderParam(shader, "diff", 0);
	GLHandler::setShaderParam(shader, "texRing", 1);
	GLHandler::setShaderParam(shader, "norm", 2);
	GLHandler::setShaderParam(shader, "atmosphere", atmosphere);

	// NOLINTNEXTLINE(cert-msc30-c, cert-msc50-c, cert-msc50-cpp)
	float seed = 10000.f * static_cast<float>(rand()) / INT_MAX;

	GLHandler::ShaderProgram sdiff = GLHandler::newShader(
	    "planet/planet", "planet/gentex/terrestrialdiff");
	GLHandler::setShaderParam(sdiff, "color", color);
	GLHandler::setShaderParam(sdiff, "polarLatitude", polarLatitude);
	GLHandler::setShaderParam(sdiff, "seed", seed);
	GLHandler::Mesh mdiff = Primitives::newUnitSphere(sdiff, 50, 50);

	envMap(sdiff, mdiff, cubemapDiffuse);

	GLHandler::deleteMesh(mdiff);
	GLHandler::deleteShader(sdiff);

	GLHandler::ShaderProgram snorm = GLHandler::newShader(
	    "planet/planet", "planet/gentex/terrestrialnorm");
	GLHandler::setShaderParam(snorm, "color", color);
	GLHandler::setShaderParam(snorm, "polarLatitude", polarLatitude);
	GLHandler::setShaderParam(snorm, "seed", seed);
	GLHandler::Mesh mnorm = Primitives::newUnitSphere(snorm, 50, 50);

	envMap(snorm, mnorm, cubemapNormal);

	GLHandler::deleteMesh(mnorm);
	GLHandler::deleteShader(snorm);
}

void Planet::initGazGiant(QColor const& color, float bandsIntensity,
                          float stormsIntensity)
{
	valid  = true;
	shader = GLHandler::newShader("planet/planet", "planet/diffplanet");
	mesh   = Primitives::newUnitSphere(shader, 50, 50);

	GLHandler::setShaderParam(shader, "diff", 0);
	GLHandler::setShaderParam(shader, "texRing", 1);
	// NOLINTNEXTLINE(cert-msc30-c, cert-msc50-c, cert-msc50-cpp)
	float seed = 10000.f * static_cast<float>(rand()) / INT_MAX;

	GLHandler::ShaderProgram sdiff
	    = GLHandler::newShader("planet/planet", "planet/gentex/gazgiantdiff");
	GLHandler::setShaderParam(sdiff, "color", color);
	GLHandler::setShaderParam(sdiff, "bandsIntensity", bandsIntensity);
	GLHandler::setShaderParam(sdiff, "stormsIntensity", stormsIntensity);
	GLHandler::setShaderParam(sdiff, "seed", seed);
	GLHandler::Mesh mdiff = Primitives::newUnitSphere(sdiff, 50, 50);

	envMap(sdiff, mdiff, cubemapDiffuse);

	GLHandler::deleteMesh(mdiff);
	GLHandler::deleteShader(sdiff);
}

void Planet::initFromTex(QString const& diffusePath)
{
	shader = GLHandler::newShader("planet/planet", "planet/diffplanet");
	mesh   = Primitives::newUnitSphere(shader, 50, 50);

	loadParallel(diffusePath, 0);
}

void Planet::initFromTex(QString const& diffusePath, QString const& normalPath,
                         float atmosphere)
{
	shader = GLHandler::newShader("planet/planet", "planet/diffnormplanet");
	mesh   = Primitives::newUnitSphere(shader, 50, 50);

	loadParallel(diffusePath, 0);
	loadParallel(normalPath, 1);
	this->atmosphere = atmosphere;
}

float Planet::updateModel(QString const& modelName)
{
	std::vector<GLHandler::Mesh> meshes;
	std::vector<GLHandler::Texture> textures;
	float modelRadius(
	    AssetLoader::loadModel(modelName, meshes, textures, shader));

	if(meshes.size() == 1)
	{
		GLHandler::deleteMesh(mesh);
		mesh = meshes[0];
	}
	else
	{
		std::cerr << "Error : Cannot import " << modelName.toStdString()
		          << "... " << meshes.size() << " models read." << std::endl;
		for(auto vMesh : meshes)
		{
			GLHandler::deleteMesh(vMesh);
		}
	}

	for(auto vTex : textures)
	{
		GLHandler::deleteTexture(vTex);
	}

	return modelRadius;
}

void Planet::updateTextureLoading()
{
	if(futures.empty())
	{
		return;
	}

	for(auto& f : futures)
	{
		if(!f.isFinished())
		{
			return;
		}
	}

	valid = true;
	GLHandler::setShaderParam(shader, "diff", 0);
	GLHandler::setShaderParam(shader, "texRing", 1);
	GLHandler::setShaderParam(shader, "atmosphere", atmosphere);

	GLHandler::ShaderProgram sdiff
	    = GLHandler::newShader("planet/planet", "planet/gentex/difftocube");
	GLHandler::Mesh mdiff = Primitives::newUnitSphere(sdiff, 50, 50);

	GLHandler::Texture tdiff = GLHandler::copyPBOToTex(pbos[0]);
	GLHandler::deletePixelBufferObject(pbos[0]);

	GLHandler::useTextures({tdiff});
	envMap(sdiff, mdiff, cubemapDiffuse);

	GLHandler::deleteTexture(tdiff);
	GLHandler::deleteMesh(mdiff);
	GLHandler::deleteShader(sdiff);

	if(futures.size() > 1)
	{
		normal        = true;
		cubemapNormal = GLHandler::newRenderTarget(
		    cubemapsSize(), cubemapsSize(), GL_RGBA16F, true);
		GLHandler::setShaderParam(shader, "norm", 2);

		GLHandler::ShaderProgram snorm
		    = GLHandler::newShader("planet/planet", "planet/gentex/normtocube");
		GLHandler::Mesh mnorm = Primitives::newUnitSphere(snorm, 50, 50);

		GLHandler::Texture tnorm = GLHandler::copyPBOToTex(pbos[1], false);
		GLHandler::deletePixelBufferObject(pbos[1]);

		GLHandler::useTextures({tnorm});
		envMap(snorm, mnorm, cubemapNormal);

		GLHandler::deleteTexture(tnorm);
		GLHandler::deleteMesh(mnorm);
		GLHandler::deleteShader(snorm);
	}
	else
	{
		GLHandler::setShaderParam(shader, "texRing", 1);
	}
	futures.resize(0);
	futures.shrink_to_fit();
}

void Planet::initRing(float innerRing, float outerRing,
                      QString const& texturePath)
{
	rings      = true;
	ringShader = GLHandler::newShader("planet/ring");
	ringMesh   = GLHandler::newMesh();

	GLHandler::setShaderParam(shader, "innerRing", innerRing);
	GLHandler::setShaderParam(shader, "outerRing", outerRing);
	GLHandler::setShaderParam(ringShader, "inner", innerRing);
	GLHandler::setShaderParam(ringShader, "outer", outerRing);
	GLHandler::setShaderParam(ringShader, "planetradius", radius);
	GLHandler::setShaderParam(ringShader, "planetoblateness", oblateness);

	float coeff(1.f / cos(3.1415 / 20.0));

	std::vector<float> ringVertices;
	std::vector<unsigned int> ringElements;
	for(unsigned int i(0); i < 20; ++i)
	{
		// 2 * i
		ringVertices.push_back(innerRing * cos(i * 3.1415f / 10.f));
		ringVertices.push_back(innerRing * sin(i * 3.1415f / 10.f));

		// 2 * i + 1
		ringVertices.push_back(outerRing * coeff * cos(i * 3.1415f / 10.f));
		ringVertices.push_back(outerRing * coeff * sin(i * 3.1415f / 10.f));

		if(i != 0)
		{
			ringElements.push_back(2 * (i - 1));
			ringElements.push_back(2 * (i - 1) + 1);
			ringElements.push_back(2 * i + 1);

			ringElements.push_back(2 * (i - 1));
			ringElements.push_back(2 * i + 1);
			ringElements.push_back(2 * i);
		}
	}
	ringElements.push_back(2 * (20 - 1));
	ringElements.push_back(2 * (20 - 1) + 1);
	ringElements.push_back(1);

	ringElements.push_back(2 * (20 - 1));
	ringElements.push_back(1);
	ringElements.push_back(0);

	GLHandler::setVertices(ringMesh, ringVertices, ringShader,
	                       {{"position", 2}}, ringElements);

	if(texturePath == "")
	{
		ringTexTarget = GLHandler::newRenderTarget(16000, 1);
		updateRing();
	}
	else
	{
		ringTextured = true;
		ringtex      = GLHandler::newTexture(texturePath.toLatin1().data());
	}
}

void Planet::updateRing()
{
	if(ringTextured)
	{
		return;
	}

	GLHandler::ShaderProgram shader
	    = GLHandler::newShader("planet/gentex/ringtex");
	GLHandler::Mesh tmpMesh = GLHandler::newMesh();
	GLHandler::setVertices(tmpMesh,
	                       {-1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f, 1.f}, shader,
	                       {{"position", 2}});

	GLHandler::setShaderParam(shader, "color", QColor(210, 180, 140));
	GLHandler::setShaderParam(
	    shader, "seed",
	    // NOLINTNEXTLINE(cert-msc30-c, cert-msc50-c, cert-msc50-cpp)
	    10000.f * static_cast<float>(rand()) / INT_MAX);

	GLHandler::beginRendering(ringTexTarget);
	GLHandler::useShader(shader);
	GLHandler::render(tmpMesh, GLHandler::PrimitiveType::TRIANGLE_STRIP);

	GLHandler::deleteMesh(tmpMesh);
	GLHandler::deleteShader(shader);
}

void Planet::renderPlanet(QVector3D const& pos, QVector3D const& lightpos,
                          std::array<QVector4D, 5> const& neighborsPosRadius,
                          std::array<QVector3D, 5> const& neighborsOblateness,
                          QMatrix4x4 const& properRotation, bool flipCoords)
{
	QMatrix4x4 model;

	model.translate(pos);
	model.scale(radius);

	renderPlanet(model, lightpos, neighborsPosRadius, neighborsOblateness,
	             properRotation, flipCoords);
}

void Planet::renderPlanet(QMatrix4x4 const& model, QVector3D const& lightpos,
                          std::array<QVector4D, 5> const& neighborsPosRadius,
                          std::array<QVector3D, 5> const& neighborsOblateness,
                          QMatrix4x4 const& properRotation, bool flipCoords)
{
	GLHandler::setShaderParam(shader, "lightpos", lightpos);
	GLHandler::setShaderParam(shader, "neighborsPosRadius", 5,
	                          &(neighborsPosRadius[0]));
	GLHandler::setShaderParam(shader, "neighborsOblateness", 5,
	                          &(neighborsOblateness[0]));
	GLHandler::setShaderParam(shader, "properRotation", properRotation);
	GLHandler::setShaderParam(shader, "oblateness", oblateness);
	if(flipCoords)
	{
		GLHandler::setShaderParam(shader, "flipCoords", QVector2D(-1.f, -1.f));
	}

	if(!normal)
	{
		if(!ringTextured)
		{
			GLHandler::useTextures(
			    {GLHandler::getColorAttachmentTexture(cubemapDiffuse),
			     GLHandler::getColorAttachmentTexture(ringTexTarget)});
		}
		else
		{
			GLHandler::useTextures(
			    {GLHandler::getColorAttachmentTexture(cubemapDiffuse),
			     ringtex});
		}
		GLHandler::useTextures({});
	}
	else
	{
		if(!ringTextured)
		{
			GLHandler::useTextures(
			    {GLHandler::getColorAttachmentTexture(cubemapDiffuse),
			     GLHandler::getColorAttachmentTexture(ringTexTarget),
			     GLHandler::getColorAttachmentTexture(cubemapNormal)});
		}
		else
		{
			GLHandler::useTextures(
			    {GLHandler::getColorAttachmentTexture(cubemapDiffuse), ringtex,
			     GLHandler::getColorAttachmentTexture(cubemapNormal)});
		}
	}
	GLHandler::setUpRender(shader, model);
	GLHandler::render(mesh);
}

void Planet::renderRings(QVector3D const& pos, QVector3D const& lightpos,
                         std::array<QVector4D, 5> const& neighborsPosRadius,
                         std::array<QVector3D, 5> const& neighborsOblateness,
                         QMatrix4x4 const& properRotation)
{
	if(!rings)
	{
		return;
	}
	QMatrix4x4 ringsModel;
	ringsModel.translate(pos);

	renderRings(ringsModel, lightpos, neighborsPosRadius, neighborsOblateness,
	            properRotation);
}

void Planet::renderRings(QMatrix4x4 const& model, QVector3D const& lightpos,
                         std::array<QVector4D, 5> const& neighborsPosRadius,
                         std::array<QVector3D, 5> const& neighborsOblateness,
                         QMatrix4x4 const& properRotation)
{
	if(!rings)
	{
		return;
	}

	GLHandler::setShaderParam(ringShader, "lightpos", lightpos);
	GLHandler::setShaderParam(ringShader, "neighborsPosRadius", 5,
	                          &(neighborsPosRadius[0]));
	GLHandler::setShaderParam(shader, "neighborsOblateness", 5,
	                          &(neighborsOblateness[0]));
	GLHandler::setShaderParam(ringShader, "properRotation", properRotation);
	GLHandler::beginTransparent();
	GLHandler::setUpRender(ringShader, model);
	if(!ringTextured)
	{
		GLHandler::useTextures(
		    {GLHandler::getColorAttachmentTexture(ringTexTarget)});
	}
	else
	{
		GLHandler::useTextures({ringtex});
	}
	GLHandler::render(ringMesh);
	GLHandler::endTransparent();
}

void Planet::loadParallel(QString const& path, unsigned int index)
{
	pbos[index] = GLHandler::newPixelBufferObject(8192, 4096);
	unsigned char* data(pbos[index].mappedData);

	futures.push_back(QtConcurrent::run([path, data]() {
		QImage img;
		if(!img.load(path))
		{
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			qWarning() << "Could not load Texture \"" << path << "\"" << '\n';
			return;
		}
		img = img.scaled(QSize(8192, 4096), Qt::IgnoreAspectRatio,
		                 Qt::SmoothTransformation)
		          .convertToFormat(QImage::Format_RGBA8888);
		std::memcpy(data, img.bits(), std::size_t(img.byteCount()));
	}));
}

void Planet::envMap(GLHandler::ShaderProgram& shader, GLHandler::Mesh& mesh,
                    GLHandler::RenderTarget& renderTarget)
{
	GLHandler::generateEnvironmentMap(renderTarget, [shader, mesh]() {
		GLHandler::setUpRender(shader);
		GLHandler::render(mesh);
	});
}

Planet::~Planet()
{
	GLHandler::deleteRenderTarget(cubemapDiffuse);
	if(!valid)
	{
		return;
	}

	if(normal)
	{
		GLHandler::deleteRenderTarget(cubemapNormal);
	}

	GLHandler::deleteMesh(mesh);
	GLHandler::deleteShader(shader);

	if(rings)
	{
		GLHandler::deleteMesh(ringMesh);
		GLHandler::deleteShader(ringShader);
		if(!ringTextured)
		{
			GLHandler::deleteRenderTarget(ringTexTarget);
		}
		else
		{
			GLHandler::deleteTexture(ringtex);
		}
	}
}
