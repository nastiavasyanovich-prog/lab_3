
#include <iostream>
#include <thread>
#include <latch>
#include <syncstream>
#include <chrono>

using namespace std;

const int nt = 3;

const int A = 4, B = 6, C = 4, D = 5, E = 8, G = 5, H = 7, I = 4, J = 5, K = 8;

latch a_done(A);
latch b_done(B);
latch c_done(C);
latch d_done(D);
latch e_done(E);
latch g_done(G);
latch h_done(H);
latch i_done(I);
latch j_done(J);

void f(char x, int i)
{
    this_thread::sleep_for(chrono::seconds(1));
    osyncstream(cout) << "From set " << x << " action " << i << " is done." << endl;
}


void run(char x, int from, int to, latch* done = nullptr)
{
    for (int i = from; i <= to; i++) {
        f(x, i);
        if (done != nullptr)
            done->count_down();
    }
}

void thread1()
{
    run('a', 1, 2, &a_done);   

    a_done.wait();             
    run('d', 1, 5, &d_done);   
    b_done.wait();             
    run('e', 5, 8, &e_done);   

    d_done.wait();             
    e_done.wait();
    run('h', 1, 5, &h_done);   

    h_done.wait();            
    i_done.wait();
    j_done.wait();
    run('k', 1, 3);            
}

void thread2()
{
    run('a', 3, 4, &a_done);   

    a_done.wait();            
    run('c', 1, 4, &c_done);  

    b_done.wait();            
    run('e', 1, 4, &e_done);  

    c_done.wait();             
    run('j', 1, 5, &j_done);  

    d_done.wait();             
    e_done.wait();
    run('h', 6, 6, &h_done);  

    h_done.wait();            
    i_done.wait();
    j_done.wait();
    run('k', 4, 6);           
}

void thread3()
{
    run('b', 1, 6, &b_done);   

    b_done.wait();            
    run('g', 1, 5, &g_done);   

    g_done.wait();             
    run('i', 1, 4, &i_done);  

    d_done.wait();            
    e_done.wait();
    run('h', 7, 7, &h_done);  

    h_done.wait();             
    i_done.wait();
    j_done.wait();
    run('k', 7, 8);           
}

int main()
{
    osyncstream(cout) << "Computation started." << endl;
    auto start = chrono::steady_clock::now();

    void (*work[nt])() = { thread1, thread2, thread3 };
    jthread threads[nt];
    for (int t = 0; t < nt; t++)
        threads[t] = jthread(work[t]);

    for (int t = 0; t < nt; t++)
        threads[t].join();

    auto finish = chrono::steady_clock::now();
    double seconds = chrono::duration<double>(finish - start).count();

    osyncstream(cout) << "Computation finished. Duration of computation: "
                      << seconds << " seconds." << endl;
    return 0;
}
