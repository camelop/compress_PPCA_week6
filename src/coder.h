#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <chrono>
using namespace std;
#define DEBUG

class Raw
{
  private:
    char *data;
    int size;
    // Disable copy
    Raw(const Raw &);
    Raw &operator=(const Raw &);

  public:
    Raw() : size(0), data(nullptr) {}
    Raw(int size) : size(size), data(nullptr)
    {
        if (size == 0)
            return;
        data = new char[size];
    }
    Raw(int size, const char *rhs) : size(size), data(nullptr)
    {
        if (size == 0)
            return;
        data = new char[size];
        for (int i = 0; i < size; ++i)
            data[i] = rhs[i];
    }
    Raw(string filename) : data(nullptr)
    {
        load(filename);
    }
    // not safe to copy!
    ~Raw()
    {
        delete[] data;
    }
    char &operator[](int idx)
    {
        return data[idx];
    }
    const char &operator[](int idx) const
    {
        return data[idx];
    }
    Raw &append(int length, const char *rhs)
    {
        // slow method to construct a Raw object, try to avoid by pre-allocation
        char *nw = new char[size + length];
        for (int i = 0; i < size; ++i)
            nw[i] = data[i];
        for (int i = 0; i < length; ++i)
            nw[i + size] = rhs[i];
        delete[] data;
        data = nw;
        return *this;
    }
    Raw &resize(int s)
    {
        delete[] data;
        size = s;
        if (s == 0)
        {
            data = nullptr;
        }
        else
        {
            data = new char[s];
        }
        return *this;
    }
    bool dump(string filename)
    {
        ofstream fout(filename, ios::binary);
        if (!fout)
            return false;
        fout.write(reinterpret_cast<const char *>(&size), sizeof(int));
        fout.write(data, size);
        fout.close();
        return true;
    }
    bool load(string filename)
    {
        ifstream fin(filename, ios::binary);
        if (!fin)
            return false;
        fin.read(reinterpret_cast<char *>(&size), sizeof(int));
        delete[] data;
        data = new char[size];
        fin.read(data, size);
        fin.close();
        return true;
    }
    void show()
    {
        cout << "size: " << size << endl;
        cout << "data: (hex-reverse)" << endl;
        int cnt = 0;
        for (int i = 0; i < size; i += sizeof(int))
        {
            int temp = 0;
            if (i + sizeof(int) < size)
            {
                temp = *(reinterpret_cast<int *>(data + i));
            }
            else
            {
                cout << "*";
                for (int j = 0; j < size - i; ++j)
                {
                    *((reinterpret_cast<char *>(&temp)) + j) = data[i + j];
                }
            }
            cout << hex << temp;
            if (cnt % 4 == 3)
                cout << "\n";
            else
                cout << "\t";
        }
        if (cnt % 4 != 3)
            cout << endl;
        else
            cout.flush();
    }
    int getSize() { return size; }
};

class Pic
{
  private:
    // Disable copy
    Pic(const Pic &);
    Pic &operator=(const Pic &);

  public:
    int h, w;
    uint8_t **data[3];
    Pic() : h(0), w(0)
    {
        data[0] = nullptr;
        data[1] = nullptr;
        data[2] = nullptr;
    }
    Pic(int h, int w) : h(h), w(w)
    {
        for (int i = 0; i < 3; ++i)
        {
            data[i] = new uint8_t *[h];
            for (int j = 0; j < h; ++j)
            {
                data[i][j] = new uint8_t[w];
            }
        }
    }
    Pic(string filename) : h(0), w(0)
    {
        data[0] = nullptr;
        data[1] = nullptr;
        data[2] = nullptr;
        fromTxt(filename);
    }
    ~Pic() { clear(); }
    bool fromTxt(string filename)
    {
        ifstream fin(filename);
        if (!fin)
            return false;
        int hh, ww;
        if (!(fin >> hh >> ww))
            return false;
        resize(hh, ww);
        for (int i = 0; i < h; ++i)
        {
            for (int j = 0; j < w; ++j)
            {
                for (int c = 0; c < 3; ++c)
                {
                    int temp;
                    if (!(fin >> temp))
                        return false;
                    data[c][i][j] = temp;
                }
            }
        }
    }
    bool toTxt(string filename)
    {
        ofstream fout(filename);
        if (!fout)
            return false;
        fout << h << ' ' << w << '\n';
        for (int i = 0; i < h; ++i)
        {
            for (int j = 0; j < w; ++j)
            {
                fout << (int)data[0][i][j] << ' ' << (int)data[1][i][j] << ' ' << (int)data[2][i][j] << '\n';
            }
        }
        fout.close();
        return true;
    }
    void clear()
    {
        for (int c = 0; c < 3; ++c)
        {
            if (h != 0)
            {
                if (w != 0)
                {
                    for (int i = 0; i < h; ++i)
                    {
                        delete[] data[c][i];
                    }
                }
                delete[] data[c];
                data[c] = nullptr;
            }
        }
    }
    void resize(int hh, int ww)
    {
        clear();
        h = hh;
        w = ww;
        for (int i = 0; i < 3; ++i)
        {
            data[i] = new uint8_t *[h];
            for (int j = 0; j < h; ++j)
            {
                data[i][j] = new uint8_t[w];
            }
        }
    }
};

class Timer
{
  private:
    chrono::high_resolution_clock::time_point last;

  public:
    double accumulated;
    Timer() : accumulated(0) {}
    void start()
    {
        last = chrono::high_resolution_clock::now();
    }
    double stop()
    {
        chrono::high_resolution_clock::time_point nw = chrono::high_resolution_clock::now();
        double secs = (chrono::duration_cast<chrono::duration<double>>(nw - last)).count();
        accumulated += secs;
        return secs;
    }
};

bool testFileEnv()
{
    Raw t(10);
    for (char i = '0'; i <= '9'; ++i)
        t[i - '0'] = i;
    string fn = "gzotpa.bin";
    if (!t.dump(fn))
        return false;
    Raw s(fn);
    //s.show();
    return true;
}

void help()
{
    cout << "Wrong args to run coder :(" << endl;
    cout << "--- Use \"./coder -e [src] [target]\" to encode your file." << endl;
    cout << "--- Use \"./coder -d [src] [target]\" to decode your file." << endl;
    cout << "--- Use \"./coder -t [src]\" to test your algorithm." << endl;
    exit(-1);
}

const double SSIM_k1 = 0.01;
const double SSIM_k2 = 0.03;
const int BITS_PER_PIXEL = 8;
const int SSIM_L = (1 << BITS_PER_PIXEL) - 1;
const int SSIM_c1 = (SSIM_k1 * SSIM_L) * (SSIM_k1 * SSIM_L);
const int SSIM_c2 = (SSIM_k2 * SSIM_L) * (SSIM_k2 * SSIM_L);
double SSIM(const Pic &p1, const Pic &p2)
{
    if (p1.h != p2.h || p1.w != p2.w)
        return -1;
    double ret = 0;
    for (int c = 0; c < 3; ++c)
    {
        double ex = 0, ey = 0, esx = 0, esy = 0, exy = 0;
        for (int i = 0; i < p1.h; ++i)
        {
            for (int j = 0; j < p1.w; ++j)
            {
                ex += p1.data[c][i][j];
                ey += p2.data[c][i][j];
                esx += p1.data[c][i][j] * p1.data[c][i][j];
                esy += p2.data[c][i][j] * p2.data[c][i][j];
                exy += p1.data[c][i][j] * p2.data[c][i][j];
            }
        }
        double total = p1.h * p1.w;
        ex /= total;
        ey /= total;
        esx /= total;
        esy /= total;
        exy /= total;
        double svx = esx - ex * ex;
        double svy = esy - ey * ey;
        double svxy = exy - ex * ey;
        ret += (2 * ex * ey + SSIM_c1) * (2 * svxy + SSIM_c2) / ((ex * ex + ey * ey + SSIM_c1) * (svx + svy + SSIM_c2));
    }
    return ret / 3;
}

void encode(const Pic &, Raw &);
void decode(const Raw &, Pic &);

int main(int argc, char *argv[])
{
#ifdef DEBUG
    if (testFileEnv())
        cerr << "File env available." << endl;
    else
        cerr << "File env unavailable, plz check your settings" << endl;
#endif
    if (argc < 3 || argc > 4)
        help();
    string param = argv[1];
    if (param != "-d" && param != "-e" && param != "-t")
        help();
    string src = argv[2];
    string des = "output";
    if (argc == 4)
        des = argv[3];
#ifdef DEBUG
    cerr << "Receive "
         << "param: [" << param << "] path: [" << src << "] -> [" << des << "]" << endl;
#endif
    if (param == "-e")
    {
        // encode txt_pic into binary
        Pic p(src);
        Raw r;
        encode(p, r);
        r.dump(des);
    }
    else if (param == "-d")
    {
        // decode binary to txt_pic
        Raw r(src);
        Pic p;
        decode(r, p);
        p.toTxt(des);
    }
    else
    {
        Timer timer;
        Pic p1(src), p2;
        Raw r;
        timer.start();
        encode(p1, r);
        timer.stop();
        double compressed_size = r.getSize();
        timer.start();
        decode(r, p2);
        timer.stop();
        // calc result
        double t = timer.accumulated;
        double size_base = 2 * sizeof(int) + p1.h * p1.w * 3;
        double accuracy = max(SSIM(p1, p2), 0.0);
        double score = pow(accuracy, 5) / (0.4 * compressed_size / size_base + 0.2) + 1.0 / (t * 8 + 0.2);
        // print result
#ifdef DEBUG
        cout << "Time   (base = 0.1s):\t\t" << t << "s";
        if (t > 0.1)
        {
            score = 0;
            cout << "\t --- Time Limit Exceeded! ---" << endl;
        }
        else
        {
            cout << "\t\tTime score: " << 1.0 / (t * 8 + 0.2) << endl;
        }
        cout << "Space  (base = " << size_base << "bytes):\t"
             << compressed_size << "b"
             << "\t\t";
        cout << "Compression ratio:\t" << compressed_size / size_base * 100 << '%' << endl;
        cout << "Accuracy (base 100" << '%' << "):\t\t" << accuracy * 100 << '%' << "\t";
        cout << "\033[33mScore:\t" << score << "\033[0m" << endl;
        cout << "**Notice**: this is an estimated result comparing to the final test"
             << endl
             << "    you may get different result due to different env." << endl;
#else
        cout << score << endl;
#endif     
    }
    return 0;
}