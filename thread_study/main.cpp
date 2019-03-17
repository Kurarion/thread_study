#include <iostream>
#include <thread>
//##chapter1##
void Hello() {
	std::cout << "Hello, world!" << std::endl;
}

//##chapter2##
//新的线程的启动总是通过构造一个std::thread对象来启动
//构造参数是一个无返回无参的函数or一个函数对象(可调用的对象)
class background_work {
	//[注意]:不要忘记使用Public声明符
public:
	background_work() = default;
	void operator()()const {
		std::cout << "Operator()" << std::endl;
	}
};
int c2_test_first() {
	//background_work w;
	//std::thread te(w);
	std::thread c2_t(*(std::unique_ptr<background_work>(new background_work())).get());
	//std::unique_ptr<background_work>x (new background_work());
	//te.join(); //ok!
	c2_t.join();//ok!
				//[注意],以下语句会被识别为一个函数的定义 
				//这个函数的参数是一个指向无参并且返回值为一个background_work对象的一个函数指针[background_work(*)()]
				//这个函数的返回值是std::thread
				//原因可能和backgroun_work的默认构造函数有关
	std::thread c2_test_this(background_work());
	//使用花括号可以避免这种问题
	std::thread c2_test{ background_work() };
	c2_test.join();

	//为了避免出现上述问题也能使用lambda表达式
	std::thread c2_new_test([] {
		std::cout << "Operator()" << std::endl;
	});
	c2_new_test.join();

	return 0;
}
//线程可以分离(detach)或是使用join进行等待
//如果主进程中没有等待线程的join就已经结束的话
//thread会的析构函数会调用std::terminate()

//detach
struct c2_detach_struct {
	int &i;
	c2_detach_struct(int &i_)
	:i(i_){

	}
	void operator()() {
		for (auto j = 0; j < 3000000; ++j)
		{
			if (j > 2888888) {
				std::cout << "2888888" << std::endl;
				std::cout << "i = " << i << std::endl;
				return;
			}
				
			
		}
	}
};
int c2_test_detach() {
	int x = 5;
	c2_detach_struct c(x);
	std::thread t(c);
	t.detach();
	std::cout << "test_detach" << std::endl;
	return 0;


	//解决出现未定义的一种方法是将新线程中所用的参数
	//自我包含一分拷贝,而对于引用那些仍需要仔细考虑
	//否则就在主线程中使用join进行等待

}
//join就不再创建函数了
//调用 join() 的行为会清除与该线程关联的任何存储
//因此std::thread对象不再关联现有的线程,它不再关联任何线程
//这意味着一旦调用了join()之后,对象就不再可连接了,joinable()将返回false

//如前面所述,需要确保在销毁std::thread对象之前调用了join()或detach()
//如果你正在分离一个线程,退学可以在线程启动后立即调用detach(),所以这不是问题
//但是如果你打算等等线程,需要仔细选择join被调用的位置
//这意味着如果在线程启动之后但在join()之前抛出异常,则可能会跳过对join()的调用
//为避免在抛出异常时终止应用程序,因此需要决定在这种情况下要执行的操作
//通常,如果打算在非特殊情况下调用join()则还需要在出现异常时调用join以避免意外的生命周期问题
void c2_f() {
	int some_local_state = 0;
	c2_detach_struct my_struct(some_local_state);
	std::thread t(my_struct);
	try {
		std::cout << "This is main thread" << std::endl;
	}
	catch (std::exception &e) {
		//为了确保发生的异常也能调用join
		t.join();
		throw;
	}
	t.join();
}
//使用RAII(资源获取是初始化时)等待一个线程完成
class thread_guard {
	std::thread &t;
public:
	explicit thread_guard(std::thread& t_) :
		t(t_)
	{}

	~thread_guard() {
		//析构函数中首先判断是否是joinable
		//这很重要,因为join()只可以被调用一次对于一个给定的要执行的线程
		if(t.joinable())
			t.join();
	}

	thread_guard(thread_guard const&) = delete;
	thread_guard& operator=(const thread_guard&) = delete;

};
void c2_f2() {
	int some_local_state = 0;
	c2_detach_struct my_struct(some_local_state);
	std::thread t(my_struct);
	thread_guard x(t);
	std::cout << "This is main thread" << std::endl;

	//当当前线程的执行到达末尾时,本地对象以相反的构造顺序被销毁
	//因此首先销毁thread_guard对象x,并在析构函数中将线程连接起来
	//,即使是因为后面的代码异常引起的退出

	//如果不需要等待线程可以调用std::detach,这会破坏线程的关联
	//并确保线程对象被销毁时不会调用std::terminate(),即使线程仍然在后台运行
}

//后台运行的线程
//一旦调用了detach会使线程一直运行在后台中,没有直接与之通信的方法,如果一个线程被分离将永远无法
//获得它的引用,因此不会再被连接,所有权和控制权转交给了c++RuntimeLibrary,并由它确保在线程退出时
//正确回收与线程关联的资源

//分离的线程经常被叫daemon threads(守护进程),这样的线程通常是长时间的
//它们几乎在应用程序的整个生命周期内运行,执行后台任务,例如监视文件系统,从对象
//缓存中清除未使用的条目或优化数据结构

//通过调用thread对象的detach()成员函数来分离线程.调用完成后,std::thread对象不再与实际的执行线程相关联,因此
//不再可连接,joinable() == false;

//为了调用detach去分离一个线程,一定要先有一个与之相关联的线程,这与Join的条件相似
//因此只有在joinable()返回true时,才能调用t.detach();

//考虑一个可以一次编辑多个文档的文字处理器应用程序
//在UI级别,和内部都有很多方法可以解决这个问题,
//目前常见的一种方法是拥有多个独立的顶级窗口,每个窗口对应一个正在编辑的文档
//虽然这些窗口看起来完全独立,但它们都在应用程序的同一个实例中运行,在内部处理此问题的一种方法是在其自己的线程运行每个文档
//编辑窗口每个线程运行相同的代码,但具有与正在编辑的文档和相应的窗口属性相关的不同数据
//因此,打开新文档需要启动新线程,处理请求的线程并不关心等待其他线程的完成,因此这使它成为运行分离
//线程的主要候选者
/*void edit_document(std::string const& filename)
{
 open_document_and_display_gui(filename);
 while(!done_editing())
 {
 user_command cmd=get_user_input(); if(cmd.type==open_new_document)
 {
 std::string const new_name=get_filename_from_user();
 std::thread t(edit_document,new_name);
 t.detach();
 }
 else
 {
 process_user_input(cmd);
 }
 }
 }
 */
//此示例中显示了将参数传递给用于启动线程的函数的情况:不仅仅将函数的名称传递给std::thread
//构造函数,还传入了filename参数,虽然可以使用其他机制来执行此操作
//,例如使用带有成员数据的函数对象而还是带参数的普通函数,但C++标准库提供了一种简单的方法 

//正如上面所示:将参数传递给可调用对象或函数基本上就像其他参数传递给std::thread构造函数一样简单,但重要的是要记住
//默认情况下参数被复制到内部存储中,可以通过新创建的执行线程访问它们,然后传递给可调用对象或函数作为值
//就像它们是临时存储一样,即使函数中的相应参数期望引用,也会执行此操作
//例子:
//void f(int i,std::string const &s);
//std::thread t(f, 3, "hello");
//这创建了一个名为t的新的线程,它调用f(3,"hello")
//请[注意],即使f将std::string作为第二个参数,字符串文字也将作为char const *传递
//并仅在新线程的上下文中转换为std::string, 当提供的参数是指向自动变量指针时
//[重要]
//void f(int i,std::string const &s);
//void oops(int some_param)
//{
//	char buffer[1024];
//	sprintf(buffer, "%i", some_param);
//	std::thread t(f, 3, buffer);
//  //std::thread t(f, 3, std::string(buffer));
//	t.detach();
//}
//在这种情况下,它是指向传递给新线程的局部变量缓冲区的指针,并且在将缓冲区转换为新线程上的std::string
//之前,oops函数很可能会退出,从而导致未定义的行为
//解决方案是将缓冲区传递给std::thread构造函数之前强制转换为std::string 
//p47[未完]
int main() {
	//##chapter1##
	//std::thread t(Hello);
	//t.join();
	//##chapter2##
	
	//c2_test_first();
	//c2_test_detach();
	//std::cout << "main" << std::endl;
	//输出为
	/*
	test_detach
	main 
	288888
	i = 3021344 //出现了未定义行为
	*/
	//c2_f();
	//c2_f2();
	system("pause");
}