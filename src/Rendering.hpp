#pragma once

#include "Game.hpp"

namespace g {
	struct Rendering : public System {
		using System::System;

		void Start() override;
		void Update(float dt) override;
		void Destroy() override;
	};
}