#pragma once

struct DeathWindowStruct {
	std::string playerName;
	unsigned int playercolor;
	std::string killerName;
	unsigned int killercolor;
	uint8_t reason;
};

class DeathWindow : public Widget {
public:
	DeathWindow();

	void record(const char* playername, unsigned int playercolor, const char* killername, unsigned int killercolor, uint8_t reason);

	virtual void draw(ImGuiRenderer* renderer) override;

private:
	const char* spriteIDForWeapon(uint8_t byteWeaponID);

private:
	std::list<DeathWindowStruct*> m_pDeathWindow;
};
