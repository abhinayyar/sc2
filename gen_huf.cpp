#include<iostream>
#include<vector>
#include<unordered_map>
#include<utility>
#include<fstream>
#include<sstream>

using namespace std;

struct Tree
{
	public:
	bool is_symbol;
	string symbol;
	Tree *left;
	Tree *right;
	int val;
	Tree(bool is,string sy)
	{
		is_symbol=is;
		symbol.assign(sy);
		val=0;
		left=NULL;
		right=NULL;		
	}
};
vector<string> split(string s,char del)
{
	stringstream ss(s);
	string item;
	vector<string> res;
	while(getline(ss,item,del))
	{
		res.push_back(item);
	}
	return res;
}
bool myfunc(pair<int,pair<Tree*,string> > a,pair<int,pair<Tree*,string> > b)
{
	return a.first<b.first;
}
Tree* gen_hf_tree(vector<pair<int,pair<Tree*,string> > >& track)
{
	if(track.size()==0) return NULL;

	if(track.size()==1 && track[0].second.first!=NULL) return track[0].second.first;

	pair<int,pair<Tree*,string> > a = track[0];
	pair<int,pair<Tree*,string> > b = track[1];
	
	track.erase(track.begin());
	track.erase(track.begin());

	Tree *node = new Tree(false,"");
	node->val=a.first+b.first;
	Tree *one_node = a.second.first;
	Tree *two_node = b.second.first;
	if(!a.second.first)
	{
		one_node = new Tree(true,a.second.second);
		one_node->val=a.first;	
	}
	if(!b.second.first)
	{
		two_node = new Tree(true,b.second.second);
		two_node->val=b.first;
	}
	
	node->left=one_node;
	node->right=two_node;

	//cout<<node<<"\t"<<one_node<<"\t"<<two_node<<endl;
	pair<int,pair<Tree*,string> > c;
	c.first=a.first+b.first;
	c.second.first=node;

	track.push_back(c);
	sort(track.begin(),track.end(),myfunc);
	
	return gen_hf_tree(track);	
					
}
void inorder_trav(Tree *root)
{
	if(!root) return;
	inorder_trav(root->left);
	cout<<root->val<<endl;
	inorder_trav(root->right);
		
}
void update_word_table(string file_name,unordered_map<string,int>& word_table)
{
	ifstream ifile;
	ifile.open(file_name);
	string input;
	while(getline(ifile,input))
	{
		vector<string> spl = split(input,' ');
		string word = spl[3];
		
		for(int i=0;i<word.size();i+=8)
		{
			string each_word = word.substr(i,8);
			if(word_table.find(each_word)!=word_table.end())
			{
				word_table[each_word]++;	
			}
			else
			{
				word_table.insert(make_pair(each_word,1));	
			}
		}
	}	
}
void traverse(Tree *root,string stack,unordered_map<string,string>& code)
{
	if(!root) return;

	if(root->is_symbol)
	{
		code.insert(make_pair(root->symbol,stack));
		return;
	}

	stack.push_back('0');
	traverse(root->left,stack,code);
	stack.pop_back();
	stack.push_back('1');
	traverse(root->right,stack,code);
	stack.pop_back();
}
void write_file(unordered_map<string,string> code)
{
	ofstream ofile;
	ofile.open("save_data.txt");
	auto lt = code.begin();
	for(;lt!=code.end();lt++)
	{
		pair<string,string> p = *lt;
		ofile<<p.first<<" "<<p.second<<endl;
	}
}
void sampling(string file_name)
{
	unordered_map<string,int> word_table;
	update_word_table(file_name,word_table);
	vector<pair<int,pair<Tree*,string> > > track;

	auto it = word_table.begin();
	
	for(;it!=word_table.end();it++)
	{
		pair<string,int> pt = *it;
		if(pt.second>0)
		{
			pair<int,pair<Tree*,string> > p;
			p.first=pt.second;
			p.second.first=NULL;
			p.second.second.assign(pt.first);
			track.push_back(p);
		}	
	}
	sort(track.begin(),track.end(),myfunc);
	Tree *root = gen_hf_tree(track);
	//inorder_trav(root);
	unordered_map<string,string> code;
	string stack;
	traverse(root,stack,code);
	write_file(code);
	//cout<<code.size()<<endl;
	
}
string ss_encode(string input,unordered_map<string,string> tracker,float& bits)
{
	// handled for 4 byte word resolution

	// 0 with HF , 1 without HF
	string output;
	for(int i=0;i<input.size();i+=8)
	{
		string ss = input.substr(i,8);
		if(tracker.find(ss)!=tracker.end())
		{
			output+= ("0" + tracker[ss]);
			bits+=(1+tracker[ss].size());	
		}
		else
		{
			cout<<"Not encode\n";
			output+=("1" + ss);
			bits+=(1+32*8);
		}	
	}
	return output;	
}
void form_tracker(unordered_map<string,string>& tracker)
{
	ifstream ifile;
	ifile.open("save_data.txt");
	
	string input;
	while(getline(ifile,input))
	{
		vector<string> tmp = split(input,' ');
		tracker.insert(make_pair(tmp[0],tmp[1]));
	}		
}
void encode(string file_name)
{
	unordered_map<string,string> tracker;
	form_tracker(tracker);
	//cout<<tracker.size()<<endl;
	ifstream ifile;
	ifile.open(file_name);
	string input;
	float bits=0;
	float org_bytes=0;
	while(getline(ifile,input))
	{
		vector<string> tmp = split(input,' ');
		org_bytes+=tmp[3].size()/2;
		string output = ss_encode(tmp[3],tracker,bits);	
	}
	cout<<"Original (bytes) : "<<org_bytes<<"\t"<<"Encode Bytes  : "<<bits/32.0<<endl;		
}
int main(int argc,char *argv[1])
{
	if(argc<3)
	{
		cout<<"Enter file name to read from  and sample(0)|encode(1)\n";
		return 0;
	}
	if(stoi(argv[2])==0)
	{
		sampling(argv[1]);
	}
	else
	{
		encode(argv[1]);
	}	
	return 0;
}

