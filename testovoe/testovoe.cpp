#include <iostream>
#include <Windows.h>
#include <vector>
#include <queue>
#include <deque>
#include <unordered_map>
#include <tuple>


int _i_sum(int n) {

    if (!n) return 0;

    if (n < 0) n = -n;

    int sum = 0;
    int m;
    while (n > 0)
    {
        m = n % 10;
        sum = sum + m;
        n = n / 10;
    }
    return sum;
}

//for debug output purposes
int unscanned = 0;
int current_index = 0;
int total_size = 0;


struct pt {
    int x, y;
};

typedef std::tuple<int, int> key_t;


uint32_t hash(PVOID value) {
    uint32_t crc = ~0;

    for (int i = 0; i < 8; i++) {
        crc ^= reinterpret_cast<byte*>(value)[i];
        for (uint8_t j = 0; j < 8; j++)
            crc = ((crc & 1)
                ? (crc >> 1) ^ 0xEDB88320
                : (crc >> 1));

    }
    return crc;
}


struct key_hash : public std::unary_function<key_t, std::size_t>
{
    std::size_t operator()(const key_t& k) const
    {
        int x = std::get<0>(k);
        int y = std::get<1>(k);

        std::vector<int> nums;

        nums.push_back(x);
        nums.push_back(y);

        ULONG64 _h = (*(ULONG64*)(nums.data())) ;

        nums.clear();

        return hash(&_h);
    }
};


class C_Ant {
    int m_X;
    int m_Y;

    std::unordered_map<key_t, bool, key_hash> m_Visited;

    bool Visited(int x, int y) {

        //auto entry = m_Visited.find( std::make_tuple(x, y));

        auto entry = m_Visited.find(std::make_tuple(x, y));

        if (entry != m_Visited.end()) {

         //   printf("Visited  %d %d\n", x, y);
         //   printf("Visited with %d %d\n\n", std::get<0>(entry->first), std::get<1>(entry->first));

           // system("pause");

            return true;

        }

        return false;
    }

    

    bool ValidateStep(int new_x, int new_y, std::queue<pt> & new_points) {

        bool duplicate = Visited(new_x, new_y);

        if (duplicate)
        {
            m_X = new_x;
            m_Y = new_y;
            return true;
        }

        int sum_x = _i_sum(new_x);
        int sum_y = _i_sum(new_y);

        //printf("%d (%d) + %d (%d) = %d <= 25 -> %d\n", sum_x, new_x, sum_y, new_y, sum_x + sum_y, (sum_x + sum_y) <= 25);
        if ((sum_x + sum_y) <= 25) {
           
            m_X = new_x;
            m_Y = new_y;
            new_points.push({m_X, m_Y});
            return true;
        }

        return false;
    }

public :
    C_Ant(int x, int y) {
        m_X = x;
        m_Y = y;
        m_Visited.clear();
        
        m_Visited[std::make_tuple(x, y)] = true;

    }

    bool MoveLeft(std::queue<pt>& new_points) {
        //printf("Moving left\n");
        if (ValidateStep(m_X - 1, m_Y, new_points)) {
            return true;
        }
        return false;
    }

    bool MoveRight(std::queue<pt>& new_points) {
        //printf("Moving right\n");
        if (ValidateStep(m_X + 1, m_Y, new_points)) {
            return true;
        }
        return false;
    }

    bool MoveUp(std::queue<pt>& new_points) {
        //printf("Moving up\n");
        if (ValidateStep(m_X, m_Y + 1, new_points)) {
            return true;
        }
        return false;
    } 

    bool MoveDown(std::queue<pt>& new_points) {
        //printf("Moving down\n");
        if (ValidateStep(m_X, m_Y - 1, new_points)) {
            return true;
        }
        return false;
    }

    // получение новых точек
    void DoMovement(int cached_x, int cached_y, std::queue<pt>& new_points) {

        // Симуляция во все стороны от посещенной точки

        m_X = cached_x; m_Y = cached_y;
         (MoveRight(new_points));

        m_X = cached_x; m_Y = cached_y;
         (MoveDown(new_points));

         m_X = cached_x; m_Y = cached_y;
         (MoveLeft(new_points));

        m_X = cached_x; m_Y = cached_y;
         (MoveUp(new_points));
    }



    void Move(int x, int y) {
       
        std::queue<pt> start_path;
        DoMovement(x, y, start_path);

        while (!start_path.empty()) {
            auto & _pt = start_path.front();
            auto _t = std::make_tuple(_pt.x, _pt.y);
            m_Visited[_t] = false;
            start_path.pop();
        }
        
        printf("First %d path built\n", m_Visited.size());

        unscanned = m_Visited.size() - 1;

        do {
            std::queue<pt> new_path;
            current_index = 0;
            total_size = m_Visited.size();
            for (auto& point : m_Visited) {
                if (!point.second) {
                    
                    int _x = std::get<0>(point.first);
                    int _y = std::get<1>(point.first);

                    DoMovement(_x, _y, new_path);

                    point.second = true;
                    unscanned--;
                    break;
                }
                current_index++;
            }
            
           // if (new_path.size())  printf("Adding %d points\n", new_path.size());

            int dup_on = 0;
            while (!new_path.empty()) {
                auto & _pt = new_path.front();
                auto _t = std::make_tuple(_pt.x, _pt.y);
                m_Visited[_t] = false;
                new_path.pop();
                unscanned++;
                dup_on++;
            }
           // printf("Ok\n");
            //Sleep(1);
        } while (GetUnscanned());

        printf("The ant can reach %d points\n", m_Visited.size());

    }

    bool GetUnscanned() {

       /* for (auto& point : m_Visited) {
            if (!point.second) {
                return true;
            }
        }*/

        //return false;

        return unscanned > 0;
    }

};

DWORD __stdcall Info(LPVOID p) {

    while (true) {
        char dbg[512] = { 0 };
        sprintf_s(dbg, "Left: %d | Working %d of %d", unscanned, current_index, total_size);
        SetConsoleTitleA(dbg);
        Sleep(50);
    }

    return 1;
}

int main()
{

    C_Ant ant(1000, 1000);

    CreateThread(0, 0, Info, 0, 0, 0);

    ant.Move(1000,1000);

    std::cin.get();

    return 0;
}
