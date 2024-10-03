#include <iostream>

using namespace std;

class Node{
	public:
		int vertex;
		int weight;
		Node *next;
};

void create(Node* head[])
{
	char ch='y';
	int v1,v2,choice,no,weight;
	Node*newNode;
	Node*temp;
	cout<<"0-Directed Graph\n";
	cout<<"1-Undirected Graph\n";
	cout<<"Enter your choice (0 or 1):\n";
	cin>>choice;
	cout<<"Enter the no. of edges:\n";
	cin>>no;
	
	
	
	for(int i = 0 ; i<no ; i++)
	{
		cout<<"\nEnter the starting node ending node and weight:\n";
		
		cin>>v1;
		cin>>v2;
		cin>>weight;
	
		newNode = new Node();
		newNode->vertex = v2;
		newNode->weight=weight;
		temp = head[v1];
		
		if(temp==NULL)
		{
			head[v1] = newNode;
		}
		else
		{
			while(temp->next!=NULL)
			{
				temp=temp->next;
			}
			temp->next = newNode;
		}
		if(choice==1)
		{
			newNode = new Node();
			newNode->vertex = v1;
			newNode->weight = weight;
			temp = head[v2];
			
			if(temp==NULL)
			{
				head[v2] = newNode;
			}
			else
			{
				while(temp->next!=NULL)
				{
					temp = temp->next;
				}
				temp->next = newNode;
			}
		}
	}
	
	
}

void display(Node*head[],int n)
{
	int v;
	Node *adj;
	cout<<"Adjancency List Is:\n";
	for(v = 0; v<n ; v++)
	{
		cout<<"Head["<<v<<"]";
		adj = head[v];
		while(adj!=NULL)
		{
			cout<<adj->vertex<<" =>weight:"<<adj->weight<<" ";
			adj = adj->next;
		}
		
		cout<<"\n";
	}
}

int main(void)
{
	char c = 'y';
	int ch,start,n,visited[10],v;
	Node *head[50];
	cout<<"No. of vertices in the graph:\n";
	cin>>n;
	for(v = 0; v<n ; v++)
	{
		head[v] = NULL;
	}
	
	create(head);
	display(head,n);
	return 0;
}