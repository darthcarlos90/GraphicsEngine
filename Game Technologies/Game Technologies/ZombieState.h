/**
	Class: ZombieState
	Description: This class represents all the states a zombie can have and their combinations
**/

class ZombieState{
public:
	ZombieState(){};
	~ZombieState(void){};

	inline bool isWandering() { return wandering; }
	inline void SetWandering(bool val) { wandering = val; }
	inline bool isEvading() { return evading; }
	inline void SetEvading(bool val) { evading = val; }
	inline bool isHunting() { return hunting; }
	inline void setHunting(bool val) { hunting = val; }
	inline bool isDead() { return dead; }
	inline void setDead(bool val) { dead = val; }
	inline bool isMoving () { return moving; }
	inline void setMovint(bool val) { moving = val; }
	inline bool isHuntingCamera() { return huntingCamera; }
	inline void setHuntingCamera(bool val) { huntingCamera = val; }


	inline void SetProperties(bool w, bool e, bool h, bool d, bool m, bool hc){
		wandering = w;
		evading = e;
		hunting = h;
		dead = d;
		moving = m;
		huntingCamera = hc;
		
	}

	

private:
	bool wandering;
	bool evading;
	bool hunting;
	bool dead;
	bool moving;
	bool huntingCamera;
};