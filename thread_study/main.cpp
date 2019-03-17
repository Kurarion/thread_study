#include <iostream>
#include <thread>
//##chapter1##
void Hello() {
	std::cout << "Hello, world!" << std::endl;
}

//##chapter2##
//�µ��̵߳���������ͨ������һ��std::thread����������
//���������һ���޷����޲εĺ���orһ����������(�ɵ��õĶ���)
class background_work {
	//[ע��]:��Ҫ����ʹ��Public������
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
				//[ע��],�������ᱻʶ��Ϊһ�������Ķ��� 
				//��������Ĳ�����һ��ָ���޲β��ҷ���ֵΪһ��background_work�����һ������ָ��[background_work(*)()]
				//��������ķ���ֵ��std::thread
				//ԭ����ܺ�backgroun_work��Ĭ�Ϲ��캯���й�
	std::thread c2_test_this(background_work());
	//ʹ�û����ſ��Ա�����������
	std::thread c2_test{ background_work() };
	c2_test.join();

	//Ϊ�˱��������������Ҳ��ʹ��lambda���ʽ
	std::thread c2_new_test([] {
		std::cout << "Operator()" << std::endl;
	});
	c2_new_test.join();

	return 0;
}
//�߳̿��Է���(detach)����ʹ��join���еȴ�
//�����������û�еȴ��̵߳�join���Ѿ������Ļ�
//thread����������������std::terminate()

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


	//�������δ�����һ�ַ����ǽ����߳������õĲ���
	//���Ұ���һ�ֿ���,������������Щ����Ҫ��ϸ����
	//����������߳���ʹ��join���еȴ�

}
//join�Ͳ��ٴ���������
//���� join() ����Ϊ���������̹߳������κδ洢
//���std::thread�����ٹ������е��߳�,�����ٹ����κ��߳�
//����ζ��һ��������join()֮��,����Ͳ��ٿ�������,joinable()������false

//��ǰ������,��Ҫȷ��������std::thread����֮ǰ������join()��detach()
//��������ڷ���һ���߳�,��ѧ�������߳���������������detach(),�����ⲻ������
//������������ȵ��߳�,��Ҫ��ϸѡ��join�����õ�λ��
//����ζ��������߳�����֮����join()֮ǰ�׳��쳣,����ܻ�������join()�ĵ���
//Ϊ�������׳��쳣ʱ��ֹӦ�ó���,�����Ҫ���������������Ҫִ�еĲ���
//ͨ��,��������ڷ���������µ���join()����Ҫ�ڳ����쳣ʱ����join�Ա��������������������
void c2_f() {
	int some_local_state = 0;
	c2_detach_struct my_struct(some_local_state);
	std::thread t(my_struct);
	try {
		std::cout << "This is main thread" << std::endl;
	}
	catch (std::exception &e) {
		//Ϊ��ȷ���������쳣Ҳ�ܵ���join
		t.join();
		throw;
	}
	t.join();
}
//ʹ��RAII(��Դ��ȡ�ǳ�ʼ��ʱ)�ȴ�һ���߳����
class thread_guard {
	std::thread &t;
public:
	explicit thread_guard(std::thread& t_) :
		t(t_)
	{}

	~thread_guard() {
		//���������������ж��Ƿ���joinable
		//�����Ҫ,��Ϊjoin()ֻ���Ա�����һ�ζ���һ��������Ҫִ�е��߳�
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

	//����ǰ�̵߳�ִ�е���ĩβʱ,���ض������෴�Ĺ���˳������
	//�����������thread_guard����x,�������������н��߳���������
	//,��ʹ����Ϊ����Ĵ����쳣������˳�

	//�������Ҫ�ȴ��߳̿��Ե���std::detach,����ƻ��̵߳Ĺ���
	//��ȷ���̶߳�������ʱ�������std::terminate(),��ʹ�߳���Ȼ�ں�̨����
}

//��̨���е��߳�
//һ��������detach��ʹ�߳�һֱ�����ں�̨��,û��ֱ����֮ͨ�ŵķ���,���һ���̱߳����뽫��Զ�޷�
//�����������,��˲����ٱ�����,����Ȩ�Ϳ���Ȩת������c++RuntimeLibrary,������ȷ�����߳��˳�ʱ
//��ȷ�������̹߳�������Դ

//������߳̾�������daemon threads(�ػ�����),�������߳�ͨ���ǳ�ʱ���
//���Ǽ�����Ӧ�ó����������������������,ִ�к�̨����,��������ļ�ϵͳ,�Ӷ���
//���������δʹ�õ���Ŀ���Ż����ݽṹ

//ͨ������thread�����detach()��Ա�����������߳�.������ɺ�,std::thread��������ʵ�ʵ�ִ���߳������,���
//���ٿ�����,joinable() == false;

//Ϊ�˵���detachȥ����һ���߳�,һ��Ҫ����һ����֮��������߳�,����Join����������
//���ֻ����joinable()����trueʱ,���ܵ���t.detach();

//����һ������һ�α༭����ĵ������ִ�����Ӧ�ó���
//��UI����,���ڲ����кܶ෽�����Խ���������,
//Ŀǰ������һ�ַ�����ӵ�ж�������Ķ�������,ÿ�����ڶ�Ӧһ�����ڱ༭���ĵ�
//��Ȼ��Щ���ڿ�������ȫ����,�����Ƕ���Ӧ�ó����ͬһ��ʵ��������,���ڲ�����������һ�ַ����������Լ����߳�����ÿ���ĵ�
//�༭����ÿ���߳�������ͬ�Ĵ���,�����������ڱ༭���ĵ�����Ӧ�Ĵ���������صĲ�ͬ����
//���,�����ĵ���Ҫ�������߳�,����������̲߳������ĵȴ������̵߳����,�����ʹ����Ϊ���з���
//�̵߳���Ҫ��ѡ��
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
//��ʾ������ʾ�˽��������ݸ����������̵߳ĺ��������:�����������������ƴ��ݸ�std::thread
//���캯��,��������filename����,��Ȼ����ʹ������������ִ�д˲���
//,����ʹ�ô��г�Ա���ݵĺ�����������Ǵ���������ͨ����,��C++��׼���ṩ��һ�ּ򵥵ķ��� 

//����������ʾ:���������ݸ��ɵ��ö�����������Ͼ��������������ݸ�std::thread���캯��һ����,����Ҫ����Ҫ��ס
//Ĭ������²��������Ƶ��ڲ��洢��,����ͨ���´�����ִ���̷߳�������,Ȼ�󴫵ݸ��ɵ��ö��������Ϊֵ
//������������ʱ�洢һ��,��ʹ�����е���Ӧ������������,Ҳ��ִ�д˲���
//����:
//void f(int i,std::string const &s);
//std::thread t(f, 3, "hello");
//�ⴴ����һ����Ϊt���µ��߳�,������f(3,"hello")
//��[ע��],��ʹf��std::string��Ϊ�ڶ�������,�ַ�������Ҳ����Ϊchar const *����
//���������̵߳���������ת��Ϊstd::string, ���ṩ�Ĳ�����ָ���Զ�����ָ��ʱ
//[��Ҫ]
//void f(int i,std::string const &s);
//void oops(int some_param)
//{
//	char buffer[1024];
//	sprintf(buffer, "%i", some_param);
//	std::thread t(f, 3, buffer);
//  //std::thread t(f, 3, std::string(buffer));
//	t.detach();
//}
//�����������,����ָ�򴫵ݸ����̵߳ľֲ�������������ָ��,�����ڽ�������ת��Ϊ���߳��ϵ�std::string
//֮ǰ,oops�����ܿ��ܻ��˳�,�Ӷ�����δ�������Ϊ
//��������ǽ����������ݸ�std::thread���캯��֮ǰǿ��ת��Ϊstd::string 
//p47[δ��]
int main() {
	//##chapter1##
	//std::thread t(Hello);
	//t.join();
	//##chapter2##
	
	//c2_test_first();
	//c2_test_detach();
	//std::cout << "main" << std::endl;
	//���Ϊ
	/*
	test_detach
	main 
	288888
	i = 3021344 //������δ������Ϊ
	*/
	//c2_f();
	//c2_f2();
	system("pause");
}