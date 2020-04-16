#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <cstdio>
#include <cmath>
#include<math.h>
#include<stdlib.h>

typedef unsigned char uint8;

struct octNode
{
	long long cnt;//��������µ���������
	long long rSum, gSum, bSum;//��ɫ��������ɫ��������ɫ�����Ҷ�ֵ�ĺ�
	bool isLeaf;//�Ƿ���Ҷ�ӽ��
	int depth;//���ڵ�����
	octNode* child[8];//8���ӽ���ָ������
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
	void FindANode_0(octNode* node, RGBQUAD *pal);
	void combineTree();
	void empty();
	void insertColor(uint8 r, uint8 g, uint8 b);						//����һ����ɫ
	uint8 generatePalette(RGBQUAD *pal);						//���ɵ�ɫ��
	uint8 generatePalette_0(RGBQUAD *pal);
private:
	octNode *root;														//�˲����ĸ�
	int colors;															//��ǰ����ɫ����
	int maxColors;														//�����ɫ��
	int pixnum;
	int remcolor;
	octNode *depthhead[9];
	octNode *panelhead;
};
octTree::octTree() {
	root = NULL;
	panelhead = NULL;
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
	panelhead = NULL;
	colors = 0;
	maxColors = maxColorNum;
	pixnum = 0;
	root = CreateOneNode(0);
	root->isLeaf = FALSE;
	for (int i = 0; i < 9; i++) {
		depthhead[i] = NULL;
	}
}

//�ͷŰ˲������ڴ�ռ�
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
octNode* octTree::Locate(int *List, int depth, octNode*FNode) {
	int n = List[depth];
	if (FNode->child[n] == NULL) {
		FNode->child[n] = CreateOneNode(depth + 1);
		if (!FNode->child[n])return NULL;
	}


	if (FNode->child[n]->isLeaf == false) {
		FNode->cnt++;
		FNode->child[n]->isLeaf = false;
		octNode*p = Locate(List, depth + 1, FNode->child[n]);
		return p;
	}
	else {
		FNode->cnt++;
		return FNode->child[n];
	}





}
//���˲��������һ�����ص���ɫ

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
	octNode*N = Locate(LocationList, 0, root);
	if (!N) {
		printf("creat a node false!");
		return;
	}
	N->rSum += r;
	N->gSum += g;
	N->bSum += b;
	//N->isLeaf = true;
	N->cnt++;
	//N->ifNew = FALSE;
	pixnum++;
	if (colors > maxColors)//���Ҷ��������������������ɫ��nMaxColors����Ҫ�ü���С��nMaxColorsΪֹ
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

	//if (N->ifNew == TRUE) {//���������ɫ��ô��ӣ�������ǽ�������
	N->rSum = r;
	N->gSum = g;
	N->bSum = b;
	N->isLeaf = true;
	N->cnt++;
	//N->ifNew = FALSE;
	pixnum++;
	//if (colors > maxColors)//���Ҷ��������������������ɫ��nMaxColors����Ҫ�ü���С��nMaxColorsΪֹ
		//combineTree();


}*/
void octTree::combineTree() {
	while (colors > maxColors) {
		//Ѱ��һ������ķ�Ҷ�ӽڵ㣬�ϲ�֮�µ��ӽڵ�
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
			if (n != NULL) {
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

//�������еİ˲�����ѡ��256����ɫ��Ϊ���յĵ�ɫ���е���ɫ
uint8 octTree::generatePalette_0(RGBQUAD *pal)
{
	//....
	printf("%d\n", colors);
	//combineTree();
	remcolor = 0;
	FindANode_0(root, pal);
	printf("%d\n", remcolor);
	return 0;
}
void octTree::FindANode_0(octNode* node, RGBQUAD *pal) {
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
				FindANode(node->child[i], pal);
			}
		}
	}
}
uint8 octTree::generatePalette(RGBQUAD *pal)
{
	//....
	printf("%d\n", colors);
	//combineTree();
	remcolor = 0;
	FindANode(root, pal);
	printf("%d\n", remcolor);//2048ɫ
	octNode*p = panelhead;
	for (int i = 0; i < 256; i++) {
		pal[i].rgbRed = (p->rSum)/(p->cnt);
		pal[i].rgbGreen = (p->gSum) / (p->cnt);
		pal[i].rgbBlue = (p->bSum) / (p->cnt);
		pal[i].rgbReserved = 0;
		p = p->next;
	}
	return 0;
}
void octTree::FindANode(octNode* node, RGBQUAD *pal) {
	if (node->isLeaf == true) {
		if (panelhead == NULL)
		{
			panelhead = node;
			if (node->next != NULL)
				printf("FindANode!node->next!=NULL!\n");
			remcolor++;
		}
		else {
			if (node->cnt >= panelhead->cnt) {
				node->next = panelhead;
				panelhead = node;
				remcolor++;
			}
			else {
				octNode*p = panelhead;
				for (; p->next != NULL; p = p->next) {
					if (node->cnt >= p->next->cnt)
						break;
				}
				node->next = p->next;
				p->next = node;
				remcolor++;
			}
		
		}
	}
	else {
		for (int i = 0; i < 8; i++) {
			if (node->child[i] != NULL) {
				FindANode(node->child[i], pal);
			}
		}
	}
}

//�ӵ�ɫ����ѡ���������ɫ��ӽ�����ɫ
uint8 selectClosestColor(uint8 r, uint8 g, uint8 b, RGBQUAD *pal)
{
	uint8 gal = 0; int min = 1000;
	for (int i = 0; i < 256; i++) {
		int m = abs(r - pal[i].rgbRed) + abs(g - pal[i].rgbGreen) + abs(b - pal[i].rgbBlue);
		if (m < min) {
			gal = i;
			min = m;
		}
	}
	return (uint8)gal;//����ĳ��ɫ���������ڵ�ɫ�����������ɫ������ֵ
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("using: exe[0], input file[1], output file[2]\n");
		return -1;
	}
	BITMAPFILEHEADER bf, *pbf;//���롢����ļ����ļ�ͷ
	BITMAPINFOHEADER bi, *pbi;//���롢����ļ�����Ϣͷ
	RGBQUAD *pRGBQuad;//�����ɵĵ�ɫ��ָ��
	uint8 *pImage;//ת�����ͼ������
	DWORD bfSize;//�ļ���С
	LONG biWidth, biHeight;//ͼ���ȡ��߶�
	DWORD biSizeImage;//ͼ��Ĵ�С�����ֽ�Ϊ��λ��ÿ���ֽ���������4��������
	unsigned long biFullWidth;//ÿ���ֽ���������4��������

	//�������ļ�
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

	//��������ļ�
	printf("Creating %s ... \n", outputName);
	if (!(fpOut = fopen(outputName, "wb")))
	{
		printf("Can't create %s!\n", outputName);
		return -1;
	}
	printf("Success!\n");

	//��ȡ�����ļ����ļ�ͷ����Ϣͷ
	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fpIn);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fpIn);

	//��ȡ�ļ���Ϣ
	biWidth = bi.biWidth;
	biHeight = bi.biHeight;
	biFullWidth = ceil(biWidth / 4.) * 4;//bmp�ļ�ÿһ�е��ֽ���������4��������
	biSizeImage = biFullWidth * biHeight;
	bfSize = biFullWidth * biHeight + 54 + 256 * 4;//ͼ���ļ��Ĵ�С�������ļ�ͷ����Ϣͷ

	//��������ļ���BITMAPFILEHEADER
	pbf = new BITMAPFILEHEADER;
	pbf->bfType = 19778;
	pbf->bfSize = bfSize;
	pbf->bfReserved1 = 0;
	pbf->bfReserved2 = 0;
	pbf->bfOffBits = 54 + 256 * 4;
	//д��BITMAPFILEHEADER
	if (fwrite(pbf, sizeof(BITMAPFILEHEADER), 1, fpOut) != 1)
	{
		printf("\nCan't write bitmap file header!\n");
		fclose(fpOut);
		return -1;
	}

	//��������ļ���BITMAPINFOHEADER
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
	//д��BITMAPFILEHEADER
	if (fwrite(pbi, sizeof(BITMAPINFOHEADER), 1, fpOut) != 1)
	{
		printf("\nCan't write bitmap info header!\n");
		fclose(fpOut);
		return -1;
	}

	//������ɫ�˲���
	printf("Building Color OctTree ...\n");
	octTree *tree;
	//tree = new octTree(2048);
	tree = new octTree(256);//�Ľ�ǰ�㷨
	uint8 RGB[3];
	//��ȡͼ����ÿ�����ص���ɫ�������������ɫ�˲���
	for (int i = 0; i < bi.biHeight; i++)
	{
		fseek(fpIn, bf.bfOffBits + i * ceil(biWidth * 3 / 4.) * 4, 0);
		for (int j = 0; j < bi.biWidth; j++)
		{
			//��ȡһ�����ص���ɫ�������������ɫ�˲���
			fread(&RGB, 3, 1, fpIn);
			//printf("%02X %02X %02X\n", RGB[2], RGB[1], RGB[0]);
			tree->insertColor(RGB[2], RGB[1], RGB[0]);
		}
	}
	printf("Success!\n");

	//���ɲ�����ɫ��
	printf("Generating palette ... \n");
	pRGBQuad = new RGBQUAD[256];
	//tree->generatePalette(pRGBQuad);
	tree->generatePalette_0(pRGBQuad);//�Ľ�ǰ�㷨
	//���256ɫ��ɫ��
	if (fwrite(pRGBQuad, 256 * sizeof(RGBQUAD), 1, fpOut) != 1)
	{
		printf("\nCan't write palette!\n");
		fclose(fpOut);
		return -1;
	}
	printf("Success!\n");

	//���ͼ������
	printf("Generating the output image ... \n");
	pImage = new uint8[biSizeImage];
	memset(pImage, 0, biSizeImage);
	for (int i = 0; i < bi.biHeight; i++)
	{
		fseek(fpIn, bf.bfOffBits + i * ceil(biWidth * 3 / 4.) * 4, 0);
		for (int j = 0; j < bi.biWidth; j++)
		{
			//��ȡһ�����ص���ɫ��������ת��λ��ɫ����ֵ
			fread(&RGB, 3, 1, fpIn);
			pImage[i*biFullWidth + j] = selectClosestColor(RGB[2], RGB[1], RGB[0], pRGBQuad);
		}
	}
	//���ͼ������
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