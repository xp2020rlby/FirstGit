#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <cstdio>
#include <cmath>
#include<math.h>
#include<stdlib.h>

typedef unsigned char uint8;

struct octNode
{
	long long cnt;//本结点以下的像素总数
	long long rSum, gSum, bSum;//红色分量、绿色分量、蓝色分量灰度值的和
	bool isLeaf;//是否是叶子结点
	//bool ifNew;
	int depth;//本节点的深度
	octNode* child[8];//8个子结点的指针数组
	octNode*next;
};
/*
typedef struct  _OctreeNode
{
	bool            blIsLeaf;                   // TRUE if node has no children
	unsigned char   inColorIndex;                       // Index of the pallette
	unsigned int    uiPixelCount;                       // Number of pixels represented by this leaf
	unsigned int    uiRedSum;                       // Sum of red components
	unsigned int    uiGreenSum;                     // Sum of green components
	unsigned int    uiBlueSum;                      // Sum of blue components
	_OctreeNode*    ptrChild[8];                        // Pointers to child nodes
	_OctreeNode*    ptrNext;                        // Pointer to next reducible node
}OctreeNode;*/
class octTree
{
public:
	octTree();
	octTree(int maxColorNum);
	~octTree();
	void FreeOctree(octNode *& Node);
	octNode* CreateOneNode(int depth);
	octNode* Locate(int *List, int depth, octNode*Node);
	void FindANode(octNode* node, RGBQUAD *pal);
	void combineTree();
	void empty();
	void insertColor(uint8 r, uint8 g, uint8 b);						//插入一个颜色
	uint8 generatePalette(RGBQUAD *pal);						//生成调色板
private:
	octNode *root;														//八叉树的根
	int colors;															//当前的颜色总数
	int maxColors;														//最大颜色数
	int pixnum;
	int remcolor;
	octNode *depthhead[9];
};
octTree::octTree() {
	root = NULL;
	colors = 0;
	maxColors = 0;
	pixnum = 0;
	root = CreateOneNode(0);
	root->isLeaf = FALSE;
	for (int i = 0; i < 9; i++) {
		depthhead[i] = NULL;
	}
}
octTree::octTree(int maxColorNum) {
	root = NULL;
	colors = 0;
	maxColors = maxColorNum;
	pixnum = 0;
	root = CreateOneNode(0);
	root->isLeaf = FALSE;
	for (int i = 0; i < 9; i++) {
		depthhead[i] = NULL;
	}
}

//释放八叉树的内存空间
octTree::~octTree()
{
	//To do....
	empty();

}
void octTree::empty() {
	FreeOctree(root);
	colors = 0;
	maxColors = 0;
	for (int i = 0; i < 9; i++) {
		depthhead[i] = NULL;
	}
}
void octTree::FreeOctree(octNode *& Node) {
//if (Node == NULL)
	if (!Node)
		return;
	for (int i = 0; i < 8; i++)
	{
		if (Node->child[i])
			FreeOctree(Node->child[i]);
	}
	delete Node;
	Node = NULL;

}
octNode* octTree::CreateOneNode(int depth) {
	octNode *p = new octNode;
	if (!p)return NULL;
	p->rSum = -1;
	p->gSum = -1;
	p->bSum = -1;
	p->cnt = 0;
	p->depth = depth;
	p->next = NULL;
	if (depth == 8)
	{
		p->isLeaf = true;
		colors++;
	}
	else {
		p->isLeaf = false;
		p->next = depthhead[depth];
		depthhead[depth] = p;
	}
		
	//p->ifNew = true;
	for (int i = 0; i < 8; i++)
		p->child[i] = NULL;
	return p;
}
octNode* octTree::Locate(int *List,int depth,octNode*FNode) {
	int n = List[depth];
	if (FNode->child[n] == NULL) {
		FNode->child[n] = CreateOneNode(depth + 1);
		if (!FNode->child[n])return NULL;
	}
		

	if (FNode->child[n]->isLeaf==false) {
		FNode->cnt++;
		FNode->child[n]->isLeaf = false;
		octNode*p= Locate(List, depth+1, FNode->child[n]);
		return p;
	}
	else {
		//if (FNode->child[n]->ifNew == true) 
			//FNode->child[n]->cnt++;
		FNode->cnt++;
		//FNode->child[n]->isLeaf = true;
		return FNode->child[n];
	}
	

		
	
	
}
//往八叉树中添加一个像素的颜色

void octTree::insertColor(uint8 r, uint8 g, uint8 b)
{
	//....
	static unsigned char map[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
	int LocationList[8] = { 0 };
	for (int dp = 0; dp < 8; dp++) {
		int shift = 7 - dp;
		int rb = ((r&map[dp]) >> shift) << 2;
		int gb = ((g&map[dp]) >> shift) << 1;
		int bb = ((b&map[dp]) >> shift);
		LocationList[dp] = rb | gb | bb;
	}
	octNode*N = Locate(LocationList, 0,root);
	if (!N) {
		printf("creat a node false!");
		return;
	}
		
	//if (N->ifNew == TRUE) {//如果是新颜色那么添加，如果不是结束即可
	N->rSum += r;
	N->gSum += g;
	N->bSum += b;
	//N->isLeaf = true;
	N->cnt ++;
	//N->ifNew = FALSE;
	pixnum++;
	if (colors > maxColors)//如果叶点数超过了最大允许的颜色数nMaxColors，需要裁剪至小于nMaxColors为止
		combineTree();
	
	
}

/*
void octTree::insertColor(uint8 r, uint8 g, uint8 b)
{
	//....
	static unsigned char map[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
	int LocationList[8] = { 0 };
	for (int dp = 0; dp < 8; dp++) {
		int shift = 7 - dp;
		int rb = ((r&map[dp]) >> shift) << 2;
		int gb = ((g&map[dp]) >> shift) << 1;
		int bb = ((b&map[dp]) >> shift);
		LocationList[dp] = rb | gb | bb;
	}

	octNode*N = root;
	for (int i = 0; i < 9; i++) {
		if (N->isLeaf == true) {
			N->cnt += 1;
			N->rSum+=
		}
	}
	if (!N) {
		printf("creat a node false!");
		return;
	}

	//if (N->ifNew == TRUE) {//如果是新颜色那么添加，如果不是结束即可
	N->rSum = r;
	N->gSum = g;
	N->bSum = b;
	N->isLeaf = true;
	N->cnt++;
	//N->ifNew = FALSE;
	pixnum++;
	//if (colors > maxColors)//如果叶点数超过了最大允许的颜色数nMaxColors，需要裁剪至小于nMaxColors为止
		//combineTree();


}*/
void octTree::combineTree() {
	while (colors > maxColors) {
		//寻找一个最深的非叶子节点，合并之下的子节点
		int L = 7;
		for (; (L > 0) && (depthhead[L] == NULL); L--);
		if (depthhead[L] == NULL) {
			printf("combineTree()!L<=0!\n");
			return;
		}
		octNode*p = depthhead[L];
		if (p->rSum != -1 && p->bSum != -1 && p->gSum != -1) {
			printf("combineTree()!node is not a treenode!\n");
			return;
		}
		p->rSum = 0; p->bSum = 0; p->gSum = 0;
		int childsum = 0, pixn = 0;
		for (int i = 0; i < 8; i++) {
			octNode*n = p->child[i];
			if (n!= NULL) {
				p->rSum += n->rSum;
				p->bSum += n->bSum;
				p->gSum += n->gSum;
				//p->cnt += n->cnt;
				pixn += n->cnt;
				childsum++;
				delete n;
				p->child[i] = NULL;
			}
		}
		if (childsum == 0) {
			printf("combineTree()!childsun==0!\n");
			return;
		}
		if (pixn != p->cnt) {
			printf("combineTree()!pixn != p->cnt!\n");
			return;
		}
		colors -= (childsum - 1);
		p->isLeaf = true;
		depthhead[L] = p->next;
		p->next = NULL;
	}
	
}

//根据现有的八叉树，选择256个颜色作为最终的调色板中的颜色
uint8 octTree::generatePalette(RGBQUAD *pal)
{
	//....
	printf("%d\n", colors);
	//combineTree();
	remcolor = 0;
	FindANode(root, pal);
	printf("%d\n", remcolor);
	return 0;
}
void octTree::FindANode(octNode* node, RGBQUAD *pal) {
	if (node->isLeaf == true) {
		pal[remcolor].rgbRed = (node->rSum) / (node->cnt);
		pal[remcolor].rgbGreen = node->gSum / node->cnt;
		pal[remcolor].rgbBlue = node->bSum / node->cnt;
		pal[remcolor].rgbReserved = 0;
		remcolor++;
	}
	else {
		for (int i = 0; i < 8; i++) {
			if (node->child[i] != NULL) {
				FindANode(node->child[i],pal);
			}
		}
	}
}

//从调色板中选出与给定颜色最接近的颜色
uint8 selectClosestColor(uint8 r, uint8 g, uint8 b, RGBQUAD *pal)
{
	uint8 gal = 0;int min = 1000;
	for (int i = 0; i < 256; i++) {
		int m = abs(r-pal[i].rgbRed)+ abs(g - pal[i].rgbGreen)+ abs(b - pal[i].rgbBlue);
		if (m < min) {
			gal = i;
			min = m;
		}
	}
	return (uint8)gal;//给定某颜色，返回其在调色板中最近似颜色的索引值
}

int main(int argc, char *argv[])	
{
	if (argc < 3)
	{
		printf("using: exe[0], input file[1], output file[2]\n");
		return -1;
	}
	BITMAPFILEHEADER bf, *pbf;//输入、输出文件的文件头
	BITMAPINFOHEADER bi, *pbi;//输入、输出文件的信息头
	RGBQUAD *pRGBQuad;//待生成的调色板指针
	uint8 *pImage;//转换后的图象数据
	DWORD bfSize;//文件大小
	LONG biWidth, biHeight;//图象宽度、高度
	DWORD biSizeImage;//图象的大小，以字节为单位，每行字节数必须是4的整数倍
	unsigned long biFullWidth;//每行字节数必须是4的整数倍

	//打开输入文件
	char *inputName, *outputName;
	FILE *fpIn, *fpOut;
	inputName = argv[1];
	outputName = argv[2];
	printf("Opening %s ... \n", inputName);
	if (!(fpIn = fopen(inputName, "rb")))
	{
		printf("Can't open %s!\n", inputName);
		return -1;
	}
	printf("Success!\n");

	//创建输出文件
	printf("Creating %s ... \n", outputName);
	if (!(fpOut = fopen(outputName, "wb")))
	{
		printf("Can't create %s!\n", outputName);
		return -1;
	}
	printf("Success!\n");

	//读取输入文件的文件头、信息头
	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fpIn);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fpIn);

	//读取文件信息
	biWidth = bi.biWidth;
	biHeight = bi.biHeight;
	biFullWidth = ceil(biWidth / 4.) * 4;//bmp文件每一行的字节数必须是4的整数倍
	biSizeImage = biFullWidth * biHeight;
	bfSize = biFullWidth * biHeight + 54 + 256 * 4;//图象文件的大小，包含文件头、信息头

	//设置输出文件的BITMAPFILEHEADER
	pbf = new BITMAPFILEHEADER;
	pbf->bfType = 19778;
	pbf->bfSize = bfSize;
	pbf->bfReserved1 = 0;
	pbf->bfReserved2 = 0;
	pbf->bfOffBits = 54 + 256 * 4;
	//写出BITMAPFILEHEADER
	if (fwrite(pbf, sizeof(BITMAPFILEHEADER), 1, fpOut) != 1)
	{
		printf("\nCan't write bitmap file header!\n");
		fclose(fpOut);
		return -1;
	}

	//设置输出文件的BITMAPINFOHEADER
	pbi = new BITMAPINFOHEADER;
	pbi->biSize = 40;
	pbi->biWidth = biWidth;
	pbi->biHeight = biHeight;
	pbi->biPlanes = 1;
	pbi->biBitCount = 8;
	pbi->biCompression = 0;
	pbi->biSizeImage = biSizeImage;
	pbi->biXPelsPerMeter = 0;
	pbi->biYPelsPerMeter = 0;
	pbi->biClrUsed = 0;
	pbi->biClrImportant = 0;
	//写出BITMAPFILEHEADER
	if (fwrite(pbi, sizeof(BITMAPINFOHEADER), 1, fpOut) != 1)
	{
		printf("\nCan't write bitmap info header!\n");
		fclose(fpOut);
		return -1;
	}

	//构建颜色八叉树
	printf("Building Color OctTree ...\n");
	octTree *tree;
	tree = new octTree(256);
	uint8 RGB[3];
	//读取图像中每个像素的颜色，并将其插入颜色八叉树
	for (int i = 0; i < bi.biHeight; i++)
	{
		fseek(fpIn, bf.bfOffBits + i * ceil(biWidth * 3 / 4.) * 4, 0);
		for (int j = 0; j < bi.biWidth; j++)
		{
			//读取一个像素的颜色，并将其插入颜色八叉树
			fread(&RGB, 3, 1, fpIn);
			//printf("%02X %02X %02X\n", RGB[2], RGB[1], RGB[0]);
			tree->insertColor(RGB[2], RGB[1], RGB[0]);
		}
	}
	printf("Success!\n");

	//生成并填充调色板
	printf("Generating palette ... \n");
	pRGBQuad = new RGBQUAD[256];
	tree->generatePalette(pRGBQuad);

	//输出256色调色板
	if (fwrite(pRGBQuad, 256 * sizeof(RGBQUAD), 1, fpOut) != 1)
	{
		printf("\nCan't write palette!\n");
		fclose(fpOut);
		return -1;
	}
	printf("Success!\n");

	//填充图像数据
	printf("Generating the output image ... \n");
	pImage = new uint8[biSizeImage];
	memset(pImage, 0, biSizeImage);
	for (int i = 0; i < bi.biHeight; i++)
	{
		fseek(fpIn, bf.bfOffBits + i * ceil(biWidth * 3 / 4.) * 4, 0);
		for (int j = 0; j < bi.biWidth; j++)
		{
			//读取一个像素的颜色，并将其转换位颜色索引值
			fread(&RGB, 3, 1, fpIn);
			pImage[i*biFullWidth + j] = selectClosestColor(RGB[2], RGB[1], RGB[0], pRGBQuad);
		}
	}
	//输出图象数据
	if (fwrite(pImage, biSizeImage, 1, fpOut) != 1)
	{
		printf("\nCan't write image data!\n");
		fclose(fpOut);

		return -1;
	}
	printf("Success!\n");


	delete tree;
	delete pbf;
	delete pbi;
	delete[] pRGBQuad;
	delete[] pImage;
	fclose(fpIn);
	fclose(fpOut);
	printf("All done!\n");
	return 0;
}