#pragma once

#include "utils.h"

class ParticleSystem 
{
public:
	class ParticleData 
	{
	public:
		vec4 position;
		vec4 velocity;
	};

	GLuint vao;
	static std::shared_ptr<Program> drawprog;
	static std::shared_ptr<Program> updateprog;
	static std::shared_ptr<ImageTexture2DArray> tex;
	int numParticles;
	int deathTimer;
	int state = ALIVE;
	vec3 initPos;

	std::shared_ptr<Buffer> particleBuff;
	ParticleSystem(int nump, vec3 pos, int death = 1000) 
	{

		if (drawprog == nullptr) 
		{
			drawprog = std::make_shared<Program>("billboard.vs", "billboard.gs", "billboard.fs");
			updateprog = std::make_shared<Program>("cs.txt");
		}

		if (tex == nullptr) 
		{
			tex = std::make_shared<ImageTexture2DArray>("powerup.ora");
		}

		numParticles = nump;
		deathTimer = death;

		std::vector<ParticleData> D(numParticles);
		for (int i = 0; i < numParticles; ++i)
		{
			D[i].position = vec4(pos, 1);

			//randrange is in utils.h
			D[i].velocity = vec4(randrange(-0.2f, 0.2f), randrange(0.6f, 0.9f), randrange(-0.2f, 0.2f), 0);

		}
		particleBuff = std::make_shared<Buffer>((D), GL_STREAM_DRAW);

		GLuint tmp[1];
		glGenVertexArrays(1, tmp);
		vao = tmp[0]; //instance variable
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuff->buffID);
		glEnableVertexAttribArray(0);
		//notice the stride is 8 floats!
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * 4, 0);
		glBindVertexArray(0);
	}

	void update(int elapsed) 
	{
		deathTimer -= elapsed;

		if (deathTimer <= 0)
			state = DEAD;

		particleBuff->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
		Program::setUniform("elapsed", elapsed*0.01f);
		Program::setUniform("gravity", vec3(0, 0, 0));
		Program::updateUniforms();
		updateprog->dispatch(numParticles / 64, 1, 1);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	}


	void draw() 
	{
		tex->bind(0); //if we want to texture the particles
		Program::setUniform("billboardSize", vec2(0.1, 0.1));
		Program::setUniform("initialPosition", initPos);
		Program::updateUniforms();
		auto oldprog = Program::current;
		drawprog->use();
		glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, numParticles);
		if (oldprog)
			oldprog->use();
	}
};