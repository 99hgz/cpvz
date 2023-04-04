#include <graphics.h>  
#include <conio.h>
#include <stdio.h>
#include <chrono>
#include <list>   
#include <vector>
#include <functional>

#define PLANT_NORMAL 100
#define ZOMBIE_MOVE 100
#define ZOMBIE_ATTACK 101
#define ANIMATION_PLAYING 100
#define ANIMATION_DONE 101
#define ANIMATION_DELETE 102
char PATH_TO_RESOURCES[] = "D:\\pvz\\pvz\\resources\\";

uint64_t gettime() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();;
}
uint64_t current_time = gettime();

int zombies_num;
int level = 1; 

namespace timer {
	class TIMER
	{
	public:
		std::function<void(void)> fun;
		uint64_t expire;
		bool is_actived;
		TIMER(uint64_t expire_, std::function<void(void)> fun1) {
			expire = expire_;
			fun = fun1;
			is_actived = false;
		}
		void active() { is_actived = true; fun();  }
	};
	std::vector<TIMER>timers;
	void add_timer(uint64_t expire_, std::function<void(void)> fun) {
		printf("add_timer\n");
		timers.push_back(TIMER(expire_, fun));
	}
	void check_timers() {
		for (auto& x : timers)
			if ((!x.is_actived) && x.expire <= current_time)
				x.active();
		for (auto it = timers.begin(); it != timers.end();) {
			if ((*it).is_actived) 
				it = timers.erase(it);
			else
				it++;
		}
	}
}

void putimagePngLight(int x, int y, IMAGE* srcimg, int light=255)//255=原图
{
	DWORD* dst = GetImageBuffer();
	DWORD* src = GetImageBuffer(srcimg);
	int src_width = srcimg->getwidth();
	int src_height = srcimg->getheight();
	int dst_width = (getwidth());
	int dst_height = (getheight());

	// 计算贴图的实际长宽
	int iwidth = (x + src_width > dst_width) ? dst_width - x : src_width;
	int iheight = (y + src_height > dst_height) ? dst_height - y : src_height;

	// 修正贴图起始位置
	dst += dst_width * y + x;

	int r, g, b;
	for (int iy = 0; iy < iheight; iy++)
	{
		for (int ix = 0; ix < iwidth; ix++)
		{
			r = (int)((GetRValue(src[ix])) * light / 255);
			g = (int)((GetGValue(src[ix])) * light / 255);
			b = (int)((GetBValue(src[ix])) * light / 255);
			dst[ix] = RGB(r, g, b);
		}
		dst += dst_width;
		src += src_width;
	}
}

void putimagePng(int x, int y, IMAGE* srcimg, UINT transparentcolor = 0x000000)
{
	// 变量初始化
	DWORD* dst = GetImageBuffer();
	DWORD* src = GetImageBuffer(srcimg);
	int src_width = srcimg->getwidth();
	int src_height = srcimg->getheight();
	int dst_width = (getwidth());
	int dst_height = (getheight());

	// 计算贴图的实际长宽
	int iwidth = (x + src_width > dst_width) ? dst_width - x : src_width;
	int iheight = (y + src_height > dst_height) ? dst_height - y : src_height;

	// 修正贴图起始位置
	dst += dst_width * y + x;

	// 修正透明色，显示缓冲区中的数据结构为 0xaarrggbb
	//transparentcolor = 0 | BGR(transparentcolor);

	// 实现透明贴图
	for (int iy = 0; iy < iheight; iy++)
	{
		for (int ix = 0; ix < iwidth; ix++)
		{
			if (ix + x < 0)continue;
			if (iy + y < 0)continue;
			if (src[ix] != transparentcolor)
				dst[ix] = src[ix];
		}
		dst += dst_width;
		src += src_width;
	}
}

void putimagePngBlued(int x, int y, IMAGE* srcimg)
{
	DWORD* dst = GetImageBuffer();
	DWORD* src = GetImageBuffer(srcimg);
	int src_width = srcimg->getwidth();
	int src_height = srcimg->getheight();
	int dst_width = (getwidth());
	int dst_height = (getheight());

	// 计算贴图的实际长宽
	int iwidth = (x + src_width > dst_width) ? dst_width - x : src_width;
	int iheight = (y + src_height > dst_height) ? dst_height - y : src_height;

	// 修正贴图起始位置
	dst += dst_width * y + x;

	int r, g, b;
	for (int iy = 0; iy < iheight; iy++)
	{
		for (int ix = 0; ix < iwidth; ix++)
		{
			if (src[ix] != 0) {
				r = (int)((GetRValue(src[ix])) * 0.3 + 254 * 0.7);
				g = (int)((GetGValue(src[ix])));
				b = (int)((GetBValue(src[ix])));
				dst[ix] = RGB(r, g, b);
			}
		}
		dst += dst_width;
		src += src_width;
	}
}

void putimagePngAlpha(IMAGE* picture, int  picture_x, int picture_y, int alpha) //255不透明
{
	DWORD* dst = GetImageBuffer();   
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); 
	int picture_width = picture->getwidth(); 
	int picture_height = picture->getheight(); 
	int graphWidth = getwidth();      
	int graphHeight = getheight();   
	int dstX = 0;    

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			if (src[srcX] == 0) {
				continue;
			}
			int sa = alpha;
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}

char* strcat1(char* dest, const char* src) {
	char* res = NULL;
	res = (char*)malloc(200);
	strcpy(res, dest);
	strcat(res, src);
	return res;
}

int mx, my, block_x, block_y;
int if_chosen_card = -1;

namespace menubar {
	RECT output_rect = { 22, 63, 76, 83 };
	IMAGE img_background;
	int offset_x = 12;
	int sun_num = 6000, menu_card_num, card_num;
	char str_sum_num[5];
	struct CARD {
		int attack, blood, price, cool_time, frame_tot;
		uint64_t frozen_timer = 0;
		char name[10];
		IMAGE card_img;
		IMAGE preview_img;
		bool available() {
			return ((sun_num >= price) && (current_time - frozen_timer) >= cool_time);
		}
		void put_image_on_menubar(int x, int y) {
			if (sun_num < price) {
				putimagePngLight(x, y, &card_img, 50);
			}else
			if ((current_time - frozen_timer)>=cool_time) {
				putimage(x, y, &card_img);
			}
			else {
				double cool_percent = (double)(current_time - frozen_timer) / (cool_time);
				int black_height = cool_percent * 69;
				static IMAGE new_img(49, 69);
				DWORD* R = GetImageBuffer(&new_img);
				DWORD* N = GetImageBuffer(&card_img);
				double f = 0.75;
				int pos = 0, r, g, b;
				for (int i = 0; i < 69 - black_height; i++) {
					for (int j = 0; j < 49; j++) {
						r = (int)((GetRValue(N[pos])) * (1 - f));
						g = (int)((GetGValue(N[pos])) * (1 - f));
						b = (int)((GetBValue(N[pos])) * (1 - f));
						R[pos] = RGB(r, g, b);
						pos++;
					}
				}
				f = 0.5;
				for (int i = 69 - black_height; i < 69; i++) {
					for (int j = 0; j < 49; j++) {
						r = (int)((GetRValue(N[pos])) * (1 - f));
						g = (int)((GetGValue(N[pos])) * (1 - f));
						b = (int)((GetBValue(N[pos])) * (1 - f));
						R[pos] = RGB(r, g, b);
						pos++;
					}
				}
				putimage(x, y, &new_img);
			}
		}
	}cards[10];
	int menu_cards[10];
	char preview_img[100];
	char filename[100];
	void init() {
		sun_num = 6000;
		for (int i = 0; i < card_num; i++)
			cards[i].frozen_timer = 0;
	}
	void add_card(int id) {
		menu_cards[menu_card_num] = id;
		menu_card_num++;
	}

	void load_assets() {
		loadimage(&img_background, strcat1(PATH_TO_RESOURCES, "graphics\\Screen\\Seed.png"));

		FILE* fp;
		fp = fopen(strcat1(PATH_TO_RESOURCES, "data\\cards.dat"), "r");
		fscanf(fp, "%d", &card_num);//读取卡数量
		for (int i = 0; i < card_num; i++) {
			fscanf(fp, "%d %d %d %s %s", &cards[i].blood ,&cards[i].price, &cards[i].cool_time,cards[i].name, preview_img);
			//printf("%d\n", strlen(cards[i].name));
			sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Cards\\card_%s.png"), cards[i].name);
			loadimage(&cards[i].card_img, filename);
			sprintf(filename, strcat1(PATH_TO_RESOURCES, "%s"), preview_img);
			loadimage(&cards[i].preview_img, filename);
		}
		fclose(fp);

		add_card(0);
		add_card(1);
		add_card(2);
		add_card(3);
		add_card(4);
		add_card(5);
		add_card(6);
	}

	void render() {
		putimagePng(offset_x, 0, &img_background);

		settextstyle(20, 0, _T("Tahoma"));
		settextcolor(BLACK);
		setbkmode(TRANSPARENT);
		sprintf_s(str_sum_num, "%d", sun_num);
		drawtext(str_sum_num, &output_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		for (int i = 0; i < menu_card_num; i++)
			cards[menu_cards[i]].put_image_on_menubar(offset_x + 80 + 50 * i, 8);

		if (if_chosen_card != -1) {
			if (block_x >= 0 && block_x <= 4 && block_y >= 0 && block_y <= 8) {
				putimagePngAlpha(&menubar::cards[if_chosen_card].preview_img, block_y * 80 + 45, block_x * 95 + 95, 100);
				//map::add_plant(if_chosen_card, block_x, block_y);
			}
			putimagePng(mx-35, my-35, &menubar::cards[if_chosen_card].preview_img);
		}
	}
}
namespace animation {
	IMAGE frame_list[8][20];
	int frame_tot[10];
	int animation_tot;
	class ANIMATION {
	public:
		int frame_num = 0, type, state, mspf,animation_suc;
		int gx, gy;
		uint64_t frame_change_timer;
		ANIMATION(int x0,int y0,int type0, int state0, int mspf0,int ans0) {
			type = type0, state = state0, mspf = mspf0;
			gx = x0, gy = y0;
			animation_suc = ans0;
			frame_change_timer = current_time;
		}
		void calc() {
			if (state == ANIMATION_PLAYING) {
				if (current_time - frame_change_timer > mspf) {
					frame_num++;
					frame_change_timer = current_time;
					if (frame_num >= frame_tot[type])
						state = ANIMATION_DONE;
				}
			}
			else if (state == ANIMATION_DONE) {
				if (current_time - frame_change_timer > animation_suc)
					state = ANIMATION_DELETE;
			}
		}
		void render() {
			int true_frame = min((frame_tot[type] - 1), frame_num);
			putimagePng(gx, gy, &frame_list[type][true_frame]);
		}
	};
	std::vector <ANIMATION> animations;
	void load_assets() {
		char folder[100], filename[100];
		FILE* fp;
		fp = fopen(strcat1(PATH_TO_RESOURCES, "data\\animation.dat"), "r");
		fscanf(fp, "%d", &animation_tot);//读取动画数量
		for (int i = 0; i < animation_tot; i++) {
			fscanf(fp, "%s %d", folder, &frame_tot[i]);
			for (int j = 0; j < frame_tot[i]; j++) {
				sprintf(filename, strcat1(PATH_TO_RESOURCES, "%s_%d.png"), folder, j);
				loadimage(&frame_list[i][j], filename);
			}
		}
		fclose(fp);
	}
	void add_animation(int x0, int y0, int type0, int state0, int mspf0, int ans0) {
		animations.push_back(ANIMATION(x0, y0, type0, state0, mspf0, ans0));
	}
	void calc() {
		for (ANIMATION &x : animations)
			x.calc();
	}
	void render() {
		for (ANIMATION x : animations)
			x.render();
	}
	void clear() {
		for (auto it = animations.begin(); it != animations.end();) {
			if ((*it).state == ANIMATION_DELETE)
				it = animations.erase(it);
			else
				it++;
		}
	}
}

namespace plant {
	void get_attack(int x, int y, int atk);
	class PLANT {
	public:
		int x, y, type, state;
		int gx, gy, blood;
		int frame = 0;
		int value[10];
		uint64_t gen_time; 
		uint64_t animate_timer = 0;
		uint64_t attack_timer = 0;
		PLANT(int x0, int y0, int type0, int state0);
		PLANT();
		void render(int offset_x, int offset_y);
		void attack();
		void get_attack(int atk);
	};
}

namespace car {
	class CAR {
	public:
		int x;
		bool actived = 0;
		uint64_t attack_timer = 0;
		CAR() {
			x = 0;
		}
		CAR(int x0) {
			x = x0;
		}
	};
}

namespace map {
	plant::PLANT map[10][10];
	car::CAR cars[10];
	IMAGE img_background;
	int offset_x = 45, offset_y = 95;
	void load_assets();
	void add_plant(int type, int x, int y);
	void render();
	bool calc_bullet_attack(int x, int gx,int type);
	std::pair<int, int> try_move(int x, int gx);
}

namespace bullet {
	class BULLET {
	public:
		int x, gx, gy, type;
		bool attacked = 0;
		uint64_t gen_time = 0;
		BULLET(int x, int x0, int y0, int type0);
		void move() {}
		void render();
		void calc_attack();
		int calc_gx();
	};
}

namespace PeaNormal {
	IMAGE frame;
	double speed = 0.332;
	void load_assets() {
		loadimage(&frame, strcat1(PATH_TO_RESOURCES, "graphics\\Bullets\\PeaNormal\\PeaNormal_0.png"));
	}
	int calc_gx(bullet::BULLET* p) {
		return (*p).gx + speed * (current_time - (*p).gen_time);
	}
	void render(bullet::BULLET* p) {
		putimagePng((*p).gx + speed * (current_time - (*p).gen_time) - 28, (*p).gy, &frame);
	}
}

namespace PeaIce {
	IMAGE frame;
	double speed = 0.332;
	void load_assets() {
		loadimage(&frame, strcat1(PATH_TO_RESOURCES, "graphics\\Bullets\\PeaIce\\PeaIce_0.png"));
	}
	int calc_gx(bullet::BULLET* p) {
		return (*p).gx + speed * (current_time - (*p).gen_time);
	}
	void render(bullet::BULLET* p) {
		putimagePng((*p).gx + speed * (current_time - (*p).gen_time) - 28, (*p).gy, &frame);
	}
}

namespace bullet {
	BULLET::BULLET(int x1, int x0, int y0, int type0)
	{
		x = x1, gx = x0, gy = y0, type = type0;
		gen_time = gettime();
	}
	void BULLET::render() {
		if (type == 0)
			PeaNormal::render(this);
		if (type == 1)
			PeaIce::render(this);
	}
	int BULLET::calc_gx() {
		return PeaNormal::calc_gx(this);
	}
	void BULLET::calc_attack() {
		bool if_attacked = map::calc_bullet_attack(x, calc_gx(), type == 0 ? 0 : 2);
		if (if_attacked)
			attacked = 1;
	}
	std::vector <BULLET> bullets;
	void add_bullet(int x, int gx, int gy, int type) {
		bullets.push_back(BULLET(x, gx, gy, type));
	}
	void calc_attack() {
		for (BULLET& x : bullets)
			x.calc_attack();
	}
	void render() {
		for (BULLET x : bullets)
			x.render();
	}
	void clear() {
		for (auto it = bullets.begin(); it != bullets.end();) {
			int gx = (*it).calc_gx();
			if ((*it).attacked || gx >= 800) { //超出屏幕清理
				if ((*it).attacked) {
					animation::add_animation(gx, (*it).gy, 0, ANIMATION_PLAYING, 10, 100);
				}
				it = bullets.erase(it);
			}
			else
				it++;
		}
	}
}

namespace zombie {
	int blood_const[10] = { 10 , 65, 28, 10, 17 };
	int attack_speed_const[10] = { 500 , 500 , 500 , 500, 500 };
	int frame_tot[10][8], zombies;
	double speed_const = 0.015;
	IMAGE frame_list[5][6][23];
	int type_tot = 1;
	void load_assets() {
		char folder[100], folder2[100], filename[150];
		FILE* fp;
		fp = fopen(strcat1(PATH_TO_RESOURCES, "data\\zombies.dat"), "r");
		fscanf(fp, "%d", &zombies);//读取卡数量
		for (int i = 0; i < zombies; i++) {
			int id1, id2, tot;
			fscanf(fp, "%s %s %d %d %d", folder, folder2, &id1, &id2, &tot);
			frame_tot[id1][id2] = tot;
			for (int j = 0; j < tot; j++) {
				sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Zombies\\%s\\%s\\%s_%d.png"), folder, folder2, folder2, j);
				loadimage(&frame_list[id1][id2][j], filename);
			}
		}
		fclose(fp);
	}

	class ZOMBIE {
	public:
		int x, type, state;
		int appearance_state = 0;
		int gx, gy;
		double accurate_x;
		int frame = 0;
		int blood;
		double speed;
		int animate_interval = 100;
		uint64_t buff_timer[10];
		uint64_t animate_timer = 0;
		uint64_t move_timer;
		uint64_t attack_timer = 0;
		ZOMBIE(int x1, int x0, int y0, int type0, int state0)
		{
			x = x1, gx = x0, gy = y0, type = type0, state = state0;
			accurate_x = gx;
			blood = blood_const[type];
			speed = (type == 4) ? 0.01 : speed_const;
			move_timer = current_time;
			animate_timer = current_time;
			memset(buff_timer, 0, sizeof buff_timer);
		}
		ZOMBIE()
		{
			x = gx = gy = state = 0;
			accurate_x = gx;
			type = -1;
			memset(buff_timer, 0, sizeof buff_timer);
		}
		double speed_scale() {
			return (buff_timer[0] > current_time)?0.5:1;
		}
		void render() {
			int frame_type = (appearance_state << 1) | (state == ZOMBIE_ATTACK);
			if (current_time - animate_timer > (animate_interval / speed_scale())) {
				frame = (frame + 1) % frame_tot[type][frame_type];
				animate_timer = current_time;
			}
			if((buff_timer[0] > current_time))
				putimagePngBlued(gx - 90, gy - 60, &frame_list[type][frame_type][frame]);
			else {
				/*if(type==4)
					putimagePng(gx - 90, gy - 60, &frame_list[type][frame_type][frame],0xffffffff);
				else*/
					putimagePng(gx - 90, gy - 60, &frame_list[type][frame_type][frame]);
			}
		}
		void get_attack(int atk,int atktype) {
			int tblood = blood;
			blood -= atk;
			printf("%d %d\n", atk, atktype);
			if (atktype == 0 || atktype == 2) { //PeaNormal or Ice
				if (type == 0 || type == 3) {
					if (tblood > 2 && blood <= 2) {
						animation::add_animation(gx, gy, 1, ANIMATION_PLAYING, 100, 100);
						appearance_state = 1;
					}
					else if (tblood > 0 && blood <= 0) {
						animation::add_animation(gx - 80, gy - 60, 2, ANIMATION_PLAYING, 100, 100);
						//appearance_state = 1;
					}
				}
				else if (type == 1) {
					if (tblood > 10 && blood <= 10) {
						appearance_state = 1;
					}
					else if (tblood > 2 && blood <= 2) {
						animation::add_animation(gx, gy, 1, ANIMATION_PLAYING, 100, 100);
						appearance_state = 2;
					}
					else if (tblood > 0 && blood <= 0) {
						animation::add_animation(gx - 80, gy - 60, 2, ANIMATION_PLAYING, 100, 100);
						//appearance_state = 1;
					}
				}
					else if (type == 2) {
						if (tblood > 10 && blood <= 10) {
							appearance_state = 1;
						}
						else if (tblood > 2 && blood <= 2) {
							animation::add_animation(gx, gy, 1, ANIMATION_PLAYING, 100, 100);
							appearance_state = 2;
						}
						else if (tblood > 0 && blood <= 0) {
							animation::add_animation(gx - 80, gy - 60, 2, ANIMATION_PLAYING, 100, 100);
							//appearance_state = 1;
						}
					}
					else if (type == 4) {
						if (tblood > 9 && blood <= 9) {
							appearance_state = 1;
							speed*= 2;
						}
						else if (tblood > 2 && blood <= 2) {
							animation::add_animation(gx, gy, 1, ANIMATION_PLAYING, 100, 100);
							appearance_state = 2;
						}
						else if (tblood > 0 && blood <= 0) {
							animation::add_animation(gx - 80, gy - 60, 2, ANIMATION_PLAYING, 100, 100);
							//appearance_state = 1;
						}
					}
			}
			if (atktype == 2) { //Ice
				buff_timer[0] = current_time + 10000;
			}
			if (atktype == 3) { //car
				animation::add_animation(gx-90, gy-60, 7, ANIMATION_PLAYING, 100, 1000);
			}
		}
		void move() {
			double movex = (current_time - move_timer) * (speed * speed_scale());
			//printf("%d\n", movex);
			move_timer = current_time;
			std::pair<int, int> code = map::try_move(x, (int)(accurate_x - movex));
			if (code.first == -1) {
				accurate_x -= movex;
				gx = (int)accurate_x;
				if (state != ZOMBIE_MOVE)
					frame = 0;
				state = ZOMBIE_MOVE;
			}
			else {
				gx = min(gx, code.first);
				accurate_x = gx;
				if (state != ZOMBIE_ATTACK)
					frame = 0;
				state = ZOMBIE_ATTACK;
			}
			if (state == ZOMBIE_ATTACK && (current_time - attack_timer) >= (attack_speed_const[type] / speed_scale())) {
				attack_timer = current_time;
				plant::get_attack(x, code.second, 1);
			}
		}
	};
}

namespace map {
	std::list <zombie::ZOMBIE> zombies;
}

namespace CherryBomb {
	int frame_tot = 7;
	int animate_interval = 120, attack_interval = 1000;
	IMAGE frame_list[8];
	char filename[100];
	void load_assets() {
		for (int i = 0; i < frame_tot; i++) {
			sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Plants\\CherryBomb\\CherryBomb_%d.png"), i);
			loadimage(&frame_list[i], filename);
		}
	}

	void attack(plant::PLANT* p) {
		if (!(current_time - (*p).gen_time>attack_interval))return;
		animation::add_animation((*p).gx, (*p).gy, 3, ANIMATION_PLAYING, 100, 100);
		for (zombie::ZOMBIE& x : (map::zombies)) {
			if (abs(x.x - (*p).x)<=1 && abs(((*p).gx+40) - x.gx) <= 120) {
				x.get_attack(100,1);
				animation::add_animation(x.gx - 80, x.gy - 60, 4, ANIMATION_PLAYING, 50, 100);
			}
		}
		(*p).blood = 0;
	}

	void render(plant::PLANT* p, int x, int y) {
		if (current_time - (*p).animate_timer > animate_interval) {
			(*p).frame = min(((*p).frame + 1),frame_tot-1) ;
			(*p).animate_timer = current_time;
		}
		putimagePng(x, y, &frame_list[(*p).frame]);
	}
}

namespace PotatoMine {
	int frame_tot[2] = { 1,8 };
	int animate_interval = 100, attack_interval = 15000;
	IMAGE frame_list[2][8];
	char filename[100];
	void load_assets() {
		for (int j = 0; j < 2; j++)
			for (int i = 0; i < frame_tot[j]; i++) {
				sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Plants\\PotatoMine\\PotatoMine_state%d\\PotatoMine_%d.png"), j, i);
				loadimage(&frame_list[j][i], filename);
			}
	}

	void attack(plant::PLANT* p) {
		if (!(current_time - (*p).gen_time > attack_interval))return;
		bool boom = 0;
		for (zombie::ZOMBIE& x : (map::zombies)) {
			if ((x.x == (*p).x) && abs(((*p).gx + 40) - x.gx) <= 50) {
				boom = 1;
				break;
			}
		}
		if (boom == 0)return;
		animation::add_animation((*p).gx, (*p).gy, 5, ANIMATION_PLAYING, 100, 200);
		for (zombie::ZOMBIE& x : (map::zombies)) {
			if ((x.x == (*p).x) && abs(((*p).gx + 40) - x.gx) <= 50) {
				x.get_attack(100, 1);
				animation::add_animation(x.gx - 80, x.gy - 60, 4, ANIMATION_PLAYING, 50, 100);
			}
		}
		(*p).blood = 0;
	}

	void render(plant::PLANT* p, int x, int y) {
		int state = (current_time - (*p).gen_time > attack_interval);
		if (current_time - (*p).animate_timer > animate_interval) {
			(*p).frame = ((*p).frame + 1) % frame_tot[state];
			(*p).animate_timer = current_time;
		}
		putimagePng(x, y, &frame_list[state][(*p).frame]);
	}
}

namespace sun {
	int animate_interval = 100;
	int frame_tot = 22;
	IMAGE frame_list[22];
	char filename[100];

	void load_assets() {
		for (int i = 0; i < frame_tot; i++) {
			sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Plants\\Sun\\Sun_%d.png"), i);
			loadimage(&frame_list[i], filename);
		}
	}

	class SUN {
	public:
		int px, py,bx,by, frame = 0;
		uint64_t animate_timer;
		SUN(int x, int y) {
			px = x;
			py = y;
			bx = px + 78;
			by = py + 78;
			animate_timer = current_time;
		}
		void render() {
			if (current_time - animate_timer > animate_interval) {
				frame = (frame + 1) % frame_tot;
				animate_timer = current_time;
			}
			putimagePng(px, py, &frame_list[frame]);
		}
	};
	std::vector<SUN>suns;
	void add_sun(int px, int py) {
		suns.push_back(SUN(px, py));
	}
	void add_random_sun() {
		int px = (rand() % 600) + 100;
		int py = (rand() % 400) + 100;
		int t = (rand() % 2000) + 5000;
		suns.push_back(SUN(px, py));
		timer::add_timer(current_time + t, sun::add_random_sun);
	}
	void render() {
		for (auto& x : suns)
			x.render();
	}
	bool collect_sun(int x, int y) {
		for (auto it = suns.begin(); it != suns.end();) {
			if (x>=(*it).px&& x <= (*it).bx&&y>= (*it).py && y <= (*it).by) {
				menubar::sun_num += 25;
				it = suns.erase(it);
				return 1;
			}
			else
				it++;
		}
		return 0;
	}
}
namespace SunFlower {
	int frame_tot=18;
	int animate_interval = 100, attack_interval = 24000;
	IMAGE frame_list[18];
	char filename[100];
	void load_assets() {
		for (int i = 0; i < frame_tot; i++) {
			sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Plants\\SunFlower\\SunFlower_%d.png"), i);
			loadimage(&frame_list[i], filename);
		}
	}

	void attack(plant::PLANT* p) {
		if ((*p).value[0] == 0) {
			if (current_time - (*p).gen_time >= 5000) {
				sun::add_sun((*p).gx, (*p).gy);
				(*p).attack_timer = current_time;
				(*p).value[0] = 1;
			}
		}
		else {
			if (current_time - (*p).attack_timer >= attack_interval) {
				sun::add_sun((*p).gx, (*p).gy);
				(*p).attack_timer = current_time;
			}
		}
	}

	void render(plant::PLANT* p, int x, int y) {
		if (current_time - (*p).animate_timer > animate_interval) {
			(*p).frame = ((*p).frame + 1) % frame_tot;
			(*p).animate_timer = current_time;
		}
		putimagePng(x, y, &frame_list[(*p).frame]);
	}
}

namespace SnowPea {
	int frame_tot = 15;
	int animate_interval = 100, attack_interval = 1360;
	int attack_range_offset_x = 40;
	IMAGE frame_list[15];
	char filename[100];
	void load_assets() {
		for (int i = 0; i < frame_tot; i++) {
			sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Plants\\SnowPea\\SnowPea_%d.png"), i);
			loadimage(&frame_list[i], filename);
		}
	}

	bool can_attack(plant::PLANT* p) {
		if (current_time - (*p).attack_timer < attack_interval)
			return 0;
		for (zombie::ZOMBIE& x : (map::zombies)) {
			if ((x.x == (*p).x) && ((*p).gx + attack_range_offset_x) <= x.gx)
				return 1;
		}
		return 0;
	}

	void attack(plant::PLANT* p) {
		if (!can_attack(p))return;
		bullet::add_bullet((*p).x, ((*p).gx + attack_range_offset_x), (*p).gy + 5, 1);
		(*p).attack_timer = current_time;
	}

	void render(plant::PLANT* p, int x, int y) {
		if (current_time - (*p).animate_timer > animate_interval) {
			(*p).frame = ((*p).frame + 1) % frame_tot;
			(*p).animate_timer = current_time;
		}
		putimagePng(x, y, &frame_list[(*p).frame]);
	}
}

namespace RepeaterPea {
	int frame_tot = 15;
	int animate_interval = 100, attack_interval = 1360 , attack_interval2 = 200;
	int attack_range_offset_x = 40;
	IMAGE frame_list[15];
	char filename[100];
	void load_assets() {
		for (int i = 0; i < frame_tot; i++) {
			sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Plants\\RepeaterPea\\RepeaterPea_%d.png"), i);
			loadimage(&frame_list[i], filename);
		}
	}

	bool can_attack(plant::PLANT* p) {
		if (current_time - (*p).attack_timer < attack_interval)
			return 0;
		for (zombie::ZOMBIE& x : (map::zombies)) {
			if ((x.x == (*p).x) && ((*p).gx + attack_range_offset_x) <= x.gx)
				return 1;
		}
		return 0;
	}

	void attack(plant::PLANT* p) {
		if (!can_attack(p))return;
		bullet::add_bullet((*p).x, ((*p).gx + attack_range_offset_x), (*p).gy + 5, 0);
		timer::add_timer(current_time + attack_interval2, std::bind(bullet::add_bullet, (*p).x, ((*p).gx + attack_range_offset_x), (*p).gy + 5, 0));
		(*p).attack_timer = current_time;
	}

	void render(plant::PLANT* p, int x, int y) {
		if (current_time - (*p).animate_timer > animate_interval) {
			(*p).frame = ((*p).frame + 1) % frame_tot;
			(*p).animate_timer = current_time;
		}
		putimagePng(x, y, &frame_list[(*p).frame]);
	}
}

namespace Peashooter {
	int frame_tot = 13;
	int animate_interval = 100, attack_interval = 1360;
	int attack_range_offset_x = 40;
	IMAGE frame_list[13];
	char filename[100];
	void load_assets() {
		for (int i = 0; i < frame_tot; i++) {
			sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Plants\\Peashooter\\Peashooter_%d.png"), i);
			loadimage(&frame_list[i], filename);
		}
	}

	bool can_attack(plant::PLANT* p) {
		if (current_time - (*p).attack_timer < attack_interval)
			return 0;
		for (zombie::ZOMBIE& x : (map::zombies)) {
			if ((x.x == (*p).x) && ((*p).gx + attack_range_offset_x) <= x.gx)
				return 1;
		}
		return 0;
	}

	void attack(plant::PLANT* p) {
		if (!can_attack(p))return;
		bullet::add_bullet((*p).x, ((*p).gx + attack_range_offset_x), (*p).gy + 5, 0);
		(*p).attack_timer = current_time;
	}

	void render(plant::PLANT* p, int x, int y) {
		if (current_time - (*p).animate_timer > animate_interval) {
			(*p).frame = ((*p).frame + 1) % frame_tot;
			(*p).animate_timer = current_time;
		}
		putimagePng(x, y, &frame_list[(*p).frame]);
	}
}
namespace WallNut {
	int frame_tot[3] = { 16,11,15 };
	int animate_interval = 100, attack_interval = 9999;
	IMAGE frame_list[3][16];
	char filename[100];
	void load_assets() {
		for(int j=0;j<3;j++)
		for (int i = 0; i < frame_tot[j]; i++) {
			sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Plants\\WallNut\\WallNut_cracked%d\\WallNut_cracked%d_%d.png"), j,j,i);
			loadimage(&frame_list[j][i], filename);
		}
	}

	void attack(plant::PLANT* p) {
		//不会攻击
	}

	void render(plant::PLANT* p, int x, int y) {
		int type = ((*p).blood > 47) ? 0 : (((*p).blood > 24) ? 1 : 2);
		if (current_time - (*p).animate_timer > animate_interval) {
			(*p).frame = ((*p).frame + 1) % frame_tot[type];
			(*p).animate_timer = current_time;
		}
		putimagePng(x, y, &frame_list[type][(*p).frame]);
	}
}

namespace plant {
	void PLANT::render(int offset_x, int offset_y) {
		if (type == 0)
			Peashooter::render(this, offset_x, offset_y);
		if(type == 1)
			CherryBomb::render(this, offset_x, offset_y);
		if (type == 2)
			WallNut::render(this, offset_x, offset_y);
		if (type == 3)
			SunFlower::render(this, offset_x, offset_y);
		if (type == 4)
			PotatoMine::render(this, offset_x, offset_y);
		if (type == 5)
			SnowPea::render(this, offset_x, offset_y);
		if (type == 6)
			RepeaterPea::render(this, offset_x, offset_y);
	}
	void PLANT::attack() {
		if (type == 0)
			Peashooter::attack(this);
		if (type==1)
			CherryBomb::attack(this);
		if (type == 2)
			WallNut::attack(this);
		if (type == 3)
			SunFlower::attack(this);
		if (type == 4)
			PotatoMine::attack(this);
		if (type == 5)
			SnowPea::attack(this);
		if (type == 6)
			RepeaterPea::attack(this);
	}
	void PLANT::get_attack(int atk) {
		blood -= atk;
	}
	PLANT::PLANT(int x0, int y0, int type0, int state0)
	{
		x = x0, y = y0, type = type0, state = state0;
		blood = menubar::cards[type].blood;
		gx = map::offset_x + 80 * y0, gy = map::offset_y + 95 * x0;
		gen_time = current_time;
		memset(value, 0, sizeof value);
	}
	PLANT::PLANT()
	{
		x = y = state =blood= 0;
		type = -1;
		gen_time = 0;
		memset(value, 0, sizeof value);
	}
	void get_attack(int x, int y, int atk) {
		map::map[x][y].get_attack(atk);
	}
}

namespace car {
	IMAGE img_car;
	double speed = 0.5;

	void load_assets() {
		loadimage(&img_car, strcat1(PATH_TO_RESOURCES, "graphics\\Screen\\car.png"));
	}

	bool can_attack(car::CAR* p) {
		for (zombie::ZOMBIE& x : (map::zombies)) {
			if ((x.x == (*p).x) &&  x.gx <= 20)
				return 1;
		}
		return 0;
	}

	void attack(car::CAR* p) {
		if ((*p).actived || !can_attack(p))return;
		(*p).actived = 1;
		(*p).attack_timer = current_time;
		for (zombie::ZOMBIE& x : (map::zombies)) {
			printf("%d %d\n", x.x, (*p).x);
			if ((x.x == (*p).x))
				timer::add_timer(current_time + (int)(x.gx/speed), std::bind(&(zombie::ZOMBIE::get_attack),&x,1000, 3));
		}
	}

	void render(car::CAR* p) {
		if ((*p).actived) {
			if (current_time - (*p).attack_timer > 2000) 
				return;
			putimagePng(-20 + speed * (current_time - (*p).attack_timer), 95 + 20 + 95 * (*p).x , &img_car);
		}
		else {
			putimagePng(-20, 95 + 20 + 95 * (*p).x, &img_car);
		}
	}

}

namespace map {
	IMAGE img_trophy;
	void load_assets() {
		loadimage(&img_background, strcat1(PATH_TO_RESOURCES, "graphics\\Items\\Background\\Background_0.jpg"));
		loadimage(&img_trophy, strcat1(PATH_TO_RESOURCES, "graphics\\Screen\\trophy.png"));
	}
	void game_start() {
		animation::add_animation(300,300, 6, ANIMATION_PLAYING, 500, 100);
		for (int i = 0; i < 5; i++)
			cars[i] = car::CAR(i);
	}
	int game_state_check() { // 1=Win 2=Lose
		if (zombies_num == 0)
			return 1;
		for (zombie::ZOMBIE& x : (map::zombies))
			if (x.gx < 0)
				return 2;
		return 0;
	}
	void add_plant(int type, int x, int y) {
		map[x][y] = plant::PLANT(x, y, type, PLANT_NORMAL);
		//menubar::sun_num -= menubar::cards[type].price;
	}
	void add_zombie(int type, int x) {
		zombies.push_back(zombie::ZOMBIE(x, 800, offset_y + 95 * x, type, ZOMBIE_MOVE));
	}

	void attack() {
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 10; j++)
				if (map[i][j].type != -1)
					map[i][j].attack();
		for (int i = 0; i < 5; i++)
			car::attack(&cars[i]);
	}
	void clear() {
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 10; j++)
				if (map[i][j].type != -1 && map[i][j].blood <= 0)
					map[i][j] = plant::PLANT();
		for (auto it = map::zombies.begin(); it != map::zombies.end();) {
			if ((*it).blood <= 0) {
				it = map::zombies.erase(it);
				zombies_num--;
			}
			else
				it++;
		}

	}

	void zombies_move() {
		for (zombie::ZOMBIE& x : (map::zombies))
			x.move();
	}
	typedef std::pair<int, int> pr;
	pr try_move(int x, int gx) {
		pr res;
		bool rr = 0;
		for (int i = 0; i < 9; i++) {
			if (map[x][i].type != -1) {
				int l = offset_x + 80 * i;
				int r = offset_x + 80 * (i + 1);
				if (gx >= l && gx <= r) {
					res = std::make_pair(r, i), rr = 1;
					break;
				}
			}
		}
		if (rr == 0)
			res = std::make_pair(-1, x);
		return res;
	}
	bool calc_bullet_attack(int r, int gx,int type) {
		for (zombie::ZOMBIE& x : (map::zombies))
			if (x.x == r && gx >= x.gx && x.blood > 0) {
				x.get_attack(1,type);
				return 1;
			}
		return 0;
	}
	RECT output_rect = { 656, 578, 750, 600 };
	char str_sum_num[10];
	void render() {
		putimage(-220, 0, &img_background);
		
		settextstyle(25, 0, _T("Tahoma"));
		settextcolor(BLACK);
		setbkmode(TRANSPARENT);
		sprintf_s(str_sum_num, "Level 1-%d", level);
		drawtext(str_sum_num, &output_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 10; j++)
				if (map[i][j].type != -1)
					map[i][j].render(offset_x + 80 * j, offset_y + 95 * i);
		for (zombie::ZOMBIE& x : (map::zombies))
			x.render();
		for (int i = 0; i < 5; i++)
			car::render(&cars[i]);
	}
}

void render() {
	map::render();
	menubar::render();
	bullet::render();
	animation::render();
	sun::render();
}

void attack() {
	map::attack();
}
void handle_mouse() {
		while (MouseHit()) {
			MOUSEMSG m = GetMouseMsg();
			mx = m.x, my = m.y;
			block_x = (my - 95) / 95;
			block_y = (mx - 45) / 80;
			switch (m.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (my <= 88) {//menubar
					int block_num = (mx - 96) / 49;
					if (block_num < menubar::menu_card_num && menubar::cards[menubar::menu_cards[block_num]].available()) {
						if_chosen_card = menubar::menu_cards[block_num];
					}
				}
				else {
					if (sun::collect_sun(mx, my))break;
					if (map::map[block_x][block_y].type == -1 && block_x >= 0 && block_x <= 4 && block_y >= 0 && block_y <= 8 && if_chosen_card!=-1 && menubar::cards[if_chosen_card].available()) {
						menubar::sun_num -= menubar::cards[if_chosen_card].price;
						menubar::cards[if_chosen_card].frozen_timer = current_time;
						map::add_plant(if_chosen_card, block_x, block_y);
					}
					if_chosen_card = -1;
				}
				break;
			case WM_RBUTTONDOWN:
				if_chosen_card = -1;
				break;
			case WM_MOUSEMOVE:
				break;
			}
		}
		
}

void load_level(int level) {
	char folder[100], filename[100];
	FILE* fp;
	sprintf(filename, strcat1(PATH_TO_RESOURCES, "data\\level%d.dat"), level);
	fp = fopen(filename, "r");
	fscanf(fp, "%d", &zombies_num);//读取僵尸数量
	for (int i = 0; i < zombies_num; i++) {
		int ts, y, type;
		fscanf(fp, "%d %d %d", &ts , &y, &type);
		timer::add_timer(current_time + ts, std::bind(map::add_zombie, type, y));
	}
	fclose(fp);
}

void main_menu_loop() {
	IMAGE Adventure_0, Adventure_1;
	int offset_x = 390, offset_y = 70;
	loadimage(&Adventure_0, strcat1(PATH_TO_RESOURCES, "graphics\\Screen\\mainmenu0.png"));
	loadimage(&Adventure_1, strcat1(PATH_TO_RESOURCES, "graphics\\Screen\\mainmenu1.png"));

	bool start_game = 0, in_start_game = 0;

	while (1) {
		while (MouseHit()) {
			MOUSEMSG m = GetMouseMsg();
			mx = m.x, my = m.y;
			switch (m.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (mx >= offset_x && mx <= offset_x + 322 && my >= offset_y && my <= offset_y + 141) {
					start_game = 1;
				}
				break;
			case WM_RBUTTONDOWN:
				break;
			case WM_MOUSEMOVE:
				if (mx >= offset_x && mx <= offset_x + 322 && my >= offset_y && my <= offset_y + 141) {
					in_start_game = 1;
				}else
					in_start_game = 0;
				break;
			}
		}
		if (start_game)break;
		if(in_start_game)
			putimagePng(1, 1, &Adventure_1);
		else
			putimagePng(0, 0, &Adventure_0);

		FlushBatchDraw();
		Sleep(30);
	}
	current_time = gettime();
}

int game_loop(int level) {

	timer::timers.clear();
	if_chosen_card = -1;
	mx = my = block_x = block_y = 0;
	menubar::init();
	animation::animations.clear();
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 10; j++)
				map::map[i][j] = plant::PLANT();
	bullet::bullets.clear();
	map::zombies.clear();
	sun::suns.clear();
	for (int i = 0; i < 5; i++)
		map::cars[i] = car::CAR(i);


	current_time = gettime();
	load_level(level);
	timer::add_timer(current_time + 7000, sun::add_random_sun);
	map::game_start();
	while (1) {
		current_time = gettime();
		//逻辑处理
		handle_mouse();
		timer::check_timers();
		map::zombies_move();
		attack();
		bullet::calc_attack();
		animation::calc();
		//清除无效目标
		map::clear();
		bullet::clear();
		animation::clear();
		//渲染画面
		render();
		int state = map::game_state_check();
		if (state)return state;
		uint64_t gt = gettime();
		FlushBatchDraw();
		Sleep(30);
	}
}

void game_pre_ani(){
	for (int i = 0; i < 20; i++)
	{
		putimage(0, 0, &map::img_background);
		FlushBatchDraw();
		Sleep(50);
	}
	for (int i = 0; i < 40; i++)
	{
		putimage(-i*5.5, 0, &map::img_background);
		FlushBatchDraw();
		Sleep(25);
	}
}

void show_win() {
	for (int i = 0; i < 40; i++) 
		{
			DWORD* dst = GetImageBuffer();
			int dst_width = (getwidth());
			int dst_height = (getheight());
			for (int iy = 0; iy < dst_height; iy++)
			{
				for (int ix = 0; ix < dst_width; ix++) {
					int sr = ((dst[ix] & 0xff0000) >> 16)*0.875+32; //获取RGB里的R
					int sg = ((dst[ix] & 0xff00) >> 8) * 0.875 + 32;   //G
					int sb = (dst[ix] & 0xff) * 0.875 + 32;              //B
					dst[ix] = (sr << 16) | (sg << 8) | sb;
				}
				dst += dst_width;
			}
			FlushBatchDraw();
			Sleep(50);
	}
}

int lose_graph[1000][1000];
void show_lose() {

	DWORD* dst = GetImageBuffer();
	int dst_width = (getwidth());
	int dst_height = (getheight());
	for (int iy = 0; iy < dst_height; iy++)
	{
		for (int ix = 0; ix < dst_width; ix++) 
			lose_graph[ix][iy] = dst[ix];
		dst += dst_width;
	}

	for (int i = 0; i < 80; i++)
	{
		int offset_x = min(i * 5.5 , 220);
		putimage(-220 + offset_x, 0, &map::img_background);
		DWORD* dst = GetImageBuffer();
		int dst_width = (getwidth());
		int dst_height = (getheight());
		for (int iy = 0; iy < dst_height; iy++)
		{
			for (int ix = offset_x; ix < dst_width; ix++) {
				dst[ix] = lose_graph[ix - offset_x][iy];
			}
			dst += dst_width;
		}
		FlushBatchDraw();
		Sleep(25);
	}
	IMAGE fail_left;
	IMAGE fail_board;
	char folder[100], filename[100];
	for (int i = 1; i < 18; i++)
	{
		int offset_x = 220;
		putimage(-220 + offset_x, 0, &map::img_background);
		DWORD* dst = GetImageBuffer();
		int dst_width = (getwidth());
		int dst_height = (getheight());
		for (int iy = 0; iy < dst_height; iy++)
		{
			for (int ix = offset_x; ix < dst_width; ix++) {
				dst[ix] = lose_graph[ix - offset_x][iy];
			}
			dst += dst_width;
		}
		FILE* fp;
		sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Screen\\fail\\%d.jpg"), i);
		loadimage(&fail_left, filename);
		putimagePng(0, 0, &fail_left);
		FlushBatchDraw();
		Sleep(166);
	}
	for (int i = 0; i < 40; i++)
	{
		DWORD* dst = GetImageBuffer();
		int dst_width = (getwidth());
		int dst_height = (getheight());
		for (int iy = 0; iy < dst_height; iy++)
		{
			for (int ix = 0; ix < dst_width; ix++) {
				int sr = ((dst[ix] & 0xff0000) >> 16) * 0.98; //获取RGB里的R
				int sg = ((dst[ix] & 0xff00) >> 8) * 0.98;   //G
				int sb = (dst[ix] & 0xff) * 0.98;              //B
				dst[ix] = (sr << 16) | (sg << 8) | sb;
			}
			dst += dst_width;
		}
		FlushBatchDraw();
		Sleep(50);
	}
	FILE* fp;
	sprintf(filename, strcat1(PATH_TO_RESOURCES, "graphics\\Screen\\fail\\ZombiesWon.png"));
	loadimage(&fail_board, filename);
	putimagePng(120, 50, &fail_board);
	for (int i = 0; i < 20; i++)
	{
		FlushBatchDraw();
		Sleep(50);
	}
}


void ask_retry() {
	IMAGE Adventure_0, Adventure_1;
	int offset_x = 293, offset_y = 346;
	loadimage(&Adventure_0, strcat1(PATH_TO_RESOURCES, "graphics\\Screen\\fail_board0.png"));
	loadimage(&Adventure_1, strcat1(PATH_TO_RESOURCES, "graphics\\Screen\\fail_board1.png"));

	bool start_game = 0, in_start_game = 0;

	while (1) {
		while (MouseHit()) {
			MOUSEMSG m = GetMouseMsg();
			mx = m.x, my = m.y;
			switch (m.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (mx >= offset_x && mx <= offset_x + 206 && my >= offset_y && my <= offset_y + 42) {
					start_game = 1;
				}
				break;
			case WM_RBUTTONDOWN:
				break;
			case WM_MOUSEMOVE:
				if (mx >= offset_x && mx <= offset_x + 206 && my >= offset_y && my <= offset_y + 42) {
					in_start_game = 1;
				}
				else
					in_start_game = 0;
				break;
			}
		}
		if (start_game)break;
		if (in_start_game)
			putimagePng(0, -100, &Adventure_1);
		else
			putimagePng(0, -100, &Adventure_0);

		FlushBatchDraw();
		Sleep(30);
	}
}

void main_loop() {
	level = 1;
	while (1) {
		game_pre_ani();
		int state = game_loop(level);
		if (state == 1) {
			show_win();
			level = level + 1;
		}
		else if (state == 2) {
			show_lose();
			ask_retry();
		}
	}
}

int main()
{
	initgraph(800, 600);		// 初始游戏窗口画面，宽 800，高 600
	BeginBatchDraw();

	main_menu_loop();

	//load assests
	map::load_assets();
	menubar::load_assets();
	Peashooter::load_assets();
	PeaNormal::load_assets();
	zombie::load_assets();
	PotatoMine::load_assets();
	animation::load_assets();
	CherryBomb::load_assets();
	sun::load_assets();
	WallNut::load_assets();
	SunFlower::load_assets();
	PeaIce::load_assets();
	RepeaterPea::load_assets();
	SnowPea::load_assets();
	car::load_assets();
	
	/*for (int i = 0; i < 5; i++)
		for (int j = 0; j < 3; j++) {
			int p = (i + j) % 3;
			map::add_plant((p==0) ? 0 : ((p==1)?5:6), i, j);
		}
	for (int i = 0; i < 5; i++)
		for (int j = 4; j <= 4; j++) {
			map::add_plant(2, i, j);
		}*/
	//map::add_plant(1, 2, 2);
	//map::add_zombie(0, 0);
	/*map::add_zombie(0, 0);
	map::add_zombie(1, 1);
	map::add_zombie(2, 2);
	map::add_zombie(3, 3);
	map::add_zombie(4, 4);*/

	main_loop();

	EndBatchDraw();
	closegraph();				// 关闭窗口
	return 0;
}