
#include <random>
#include <cctype>
#include <cstdio>


template<class Gen>
void print_pass(int len, Gen& gen)
{
    auto dist = std::uniform_int_distribution(0, 255);

    auto get_next = [&]()
    {
        unsigned char c;
        do
        {
            c = dist(gen);

        } while (!std::isprint(c));

        return c;
    };

    for (int i = 0; i < len; ++i)
    {
        unsigned char c = get_next();
        std::printf("%c", c);
    }
    std::printf("\n");
}


int main()
{
    auto dev = std::random_device{};
    auto gen = std::mt19937(dev());

    for(int i = 0; i < 6; ++i)
        print_pass(16, gen);

    return 0;
}
