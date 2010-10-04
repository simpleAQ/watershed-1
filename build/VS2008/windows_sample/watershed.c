#include "watershed.h"
#include <stdlib.h>
#include <string.h>

linked_contrast_t *contrast[256];
linked_region_t *root_region;
linked_pixel_t *border;

point_t *init_point(){
	point_t *p;

	p = (point_t *)malloc(sizeof(point_t));

	return p;
}

linked_pixel_t *init_linked_pixel(){
	linked_pixel_t *pixel;
	
	pixel = (linked_pixel_t *)malloc(sizeof(linked_pixel_t));
	pixel->p = init_point();
	pixel->next = NULL;
	
	return pixel;
}

linked_region_t *init_linked_region(){
	linked_region_t *region;
	//if (region == NULL) {	// �ʱ��� region�� ���� NULL�ΰ�?
		region = (linked_region_t *)malloc(sizeof(linked_region_t));
		region->next = NULL;
		region->lpx = init_linked_pixel();
	//}

	return region;
}

void insert_region_list(linked_pixel_t *lp){
	linked_region_t *lrg = (linked_region_t *)malloc(sizeof(linked_region_t));
	lrg->lpx = lp;

	if (root_region == NULL){
		root_region = lrg;
		lrg->next = NULL;
	} else {
		lrg->next = root_region;
		root_region = lrg;
	}
}

/**
 * @brief Insert Pixel list
 * ����Ʈ p�� ������ ��ũ������Ʈ near_at_p_lpx�� ������ near_at_p_lpx ������ p�� �����Ų��.
 * @param near_at_p_lpx		IN	Parameter, Mandatory, Linked-point near at the point
 * @param p					IN	Parameter, Mandatory, the point(x,y)
 */
void insert_pixel_list(linked_pixel_t *near_at_p_lpx, point_t *p){
	linked_pixel_t *lpx;
	lpx = init_linked_pixel();
	//lpx->p = p;		// ����ü���� �̷��� ��� �ϳ�?
	lpx->p->x = p->x;
	lpx->p->y = p->y;

	lpx->next = near_at_p_lpx->next;
	near_at_p_lpx->next = lpx;
}
void merge_pixel_list(linked_pixel_t *near_at_p_lpx, linked_pixel_t *head, linked_pixel_t *tail){
	if(near_at_p_lpx->next == NULL) {
		near_at_p_lpx->next = head;
	}
	else {
		tail->next = near_at_p_lpx->next;
		near_at_p_lpx->next = head;
	}
}

//void insert_dam(point_t *p){
//	linked_pixel_t *lpx;
//	lpx = init_linked_pixel();
//	lpx->p->x = p->x;
//	lpx->p->y = p->y;
//	if (border == NULL){	// �ʱ⿡ border�� NULL�� �´°�?
//		border = lpx;
//	} else {
//		lpx->next = border;
//		border = lpx;
//	}
//}

void insert_dam(linked_pixel_t *head, linked_pixel_t *tail){
	if (border == NULL){	// �ʱ⿡ border�� NULL�� �´°�?
		border = head;
	} else {
		tail->next = border->next;
		border->next = head;
	}
}

/* int i�� �ش��ϴ� contrast�� ���ε��� c�� ����
 * contrast[i]�� null�̸� ����� �� ��.
 * LIFO��
 */
void add_linked_contrast(int x, int y, int i){

	linked_contrast_t *ct = (linked_contrast_t *)malloc(sizeof(linked_contrast_t));
	ct->p = init_point();
	ct->p->x = x;
	ct->p->y = y;

	// init
	if (contrast[i] == NULL) {
		contrast[i] = ct;
		contrast[i]->next = NULL;
	}
	else
	{
		ct->next = contrast[i];
		contrast[i] = ct;
	}
}

linked_contrast_t *get_linked_contrast(int i){

	linked_contrast_t *ct;
	
	if (contrast[i] == NULL) {
		return NULL;
	}
	else
	{
		ct = contrast[i];
		contrast[i] = contrast[i]->next;
		return ct;
	}
}

/** �� ����Ʈ�� ������ �˻�
 * @result 1 = True, 2 = False
 * @param p1	IN, Mandatory, 1st point
 * @param p2	IN, Mandatory, 2nd point
 */
int adjoin_points(point_t *p1, point_t *p2){
	if((p1->x <= p2->x+1) && (p1->x >= p2->x-1) && (p1->y <= p2->y+1) && (p1->y >= p2->y-1)) {
		return TRUE;
	} else {
		return FALSE;
	}
}


//void compute_region(point_t *p, int width, int height){
//	int duplicate_count = 0;
//	linked_region_t *lrg;
//	linked_pixel_t *lpx;
//
//	linked_pixel_t *near_at_p_lpx = NULL;
//
//	lrg = root_region;
//
//	if (lrg == NULL)
//	{
//		// root�� ���ٸ� �ٷ� ���ο� region ����
//		insert_region_list(p);
//	} else {
//		do {
//			lpx = lrg->lpx;
//			do {
//				/* �� �����ϴ� region�� ������ �ȼ��� ��� */
//				if((lpx->p->x <= p->x+1) && (lpx->p->x >= p->x-1) && (lpx->p->y <= p->y+1) && (lpx->p->y >= p->y-1)) {
//					duplicate_count++;	// �����ϴ� �ȼ� ī��Ʈ + 1
//					near_at_p_lpx = lpx;
//					break;
//				}
//				lpx = lpx->next;
//			} while (lpx != NULL);
//
//			if (duplicate_count > 1)	// �Է¹��� point �ֺ� �ȼ��� 2�� �̻��̶�� ����
//				break;
//
//			lrg = lrg->next;
//		} while (lrg != NULL);
//
//		/* ������ region�� ���� ��� */
//		switch (duplicate_count)
//		{
//			case 0:
//				// ���ο� region ����
//				insert_region_list(p);
//				break;
//			case 1:
//				// ���� region�� �ش� �ȼ� ���Խ�Ŵ
//				insert_pixel_list(near_at_p_lpx, p);
//				break;
//			case 2:
//				// �� �߻�
//				insert_dam(p);
//				break;
//			default:
//				break;
//		}
//	}
//
//	duplicate_count = 0;
//}

void compute_region(linked_contrast_t *cont, int width, int height){
	linked_contrast_t *c;
	linked_contrast_t *prev_c;
	linked_region_t *lrg;
	linked_pixel_t *lpx;
	linked_pixel_t *lpx_for_cont;
	linked_pixel_t *head;
	linked_pixel_t *tail;
	int duplicate_count = 0;
	int adjoin_count = 0;
	int connect_flag = 0;

	linked_pixel_t *near_at_p_lpx = NULL;

	while(cont != NULL)
	{
		c = cont;
		head = init_linked_pixel();
		lpx = tail = head;
		head->p->x = c->p->x;
		head->p->y = c->p->y;
		adjoin_count = 1;
		while(adjoin_count > 0){
			adjoin_count = 0;
			lpx = head;
			while (lpx != NULL){
				prev_c = cont;
				c = cont->next;
				while (c != NULL)
				{
					if(adjoin_points(lpx->p, c->p))
					{
						insert_pixel_list(tail, c->p);
						if (tail->next != NULL)
							tail = tail->next;
						prev_c->next = c->next;
						free(c);	// ����� constant free ��Ŵ
						c = prev_c->next;
						adjoin_count++;
					} else {
						prev_c = c;
						c = c->next;
					}
				}
				lpx = lpx->next;
			}
		}

		c = cont;
		cont = cont->next;
		free(c);	// head�� ���Ե� constant free��Ŵ
		c = NULL;


		// watershed �˰��� �����ϱ� ����
		// ���� constant���� ����� ��� watershed �˰��� �����ϱ�
		lrg = root_region;
		if (lrg == NULL)
		{
			// root�� ���ٸ� �ٷ� ���ο� region ����
			insert_region_list(head);
		} else {
			do {
				lpx = lrg->lpx;
				do {
					lpx_for_cont = head;
					do 
					{
						/* �� �����ϴ� region�� ������ �ȼ��� ��� */
						if(adjoin_points(lpx->p, lpx_for_cont->p)) {
							connect_flag = 1;
							near_at_p_lpx = lpx;
							break;
						}
						lpx_for_cont = lpx_for_cont->next;
					} while (lpx_for_cont != NULL);
					if (connect_flag == 1) {
						duplicate_count++;	// �����ϴ� �ȼ� ī��Ʈ + 1
						connect_flag = 0;
						break;
					}
					lpx = lpx->next;
				} while (lpx != NULL);

				if (duplicate_count > 1)	// �Է¹��� point �ֺ� �ȼ��� 2�� �̻��̶�� ����
					break;

				lrg = lrg->next;
			} while (lrg != NULL);

			/* ������ region�� ���� ��� */
			switch (duplicate_count)
			{
			case 0:
				// ���ο� region ����
				insert_region_list(head);
				break;
			case 1:
				// ���� region�� �ش� �ȼ� ���Խ�Ŵ
				merge_pixel_list(near_at_p_lpx, head, tail);
				break;
			case 2:
				// �� �߻�
				insert_dam(head, tail);
				break;
			default:
				break;
			}
		}

		duplicate_count = 0;

		// watershed �˰��� �����ϱ� ��.
	}

}

int watershed_operation(unsigned char *src, unsigned char *dst, int width, int height){
	int v, h, i;
	//linked_contrast_t *c;
	linked_region_t *lrg;
	linked_pixel_t *lpx;

	linked_region_t *lrg_for_free;
	linked_pixel_t *lpx_for_free;
	
	// ��ϼ������ �迭�� ť��
	for (v = 0; v < height; v++)
	{
		for (h = 0; h < width; h++)
		{
			add_linked_contrast(h, v, src[v*width+h]);
		}
	}
	
	for (i = 0; i < 256; i++)
	{
		//c = get_linked_contrast(i);
		//while(c != NULL){	// i�� �ش��ϴ� level�� ����� ������ �ȼ��� ��� dequeue�ϰ��� i+1�� dequeue�ϴ� ����.
		//	compute_region(c->p, width, height);

		//	free(c); //
		//	c = get_linked_contrast(i); // ������ dequeue�� null�̹Ƿ� free�ڿ� �;� ��.
		//}

		compute_region(contrast[i], width, height);
		if (contrast[i]!=NULL) {
			//free(contrast[i]);
			contrast[i] = NULL;
		}
	}

	lrg = root_region;
	while(lrg != NULL) {
		lpx = lrg->lpx;
		while(lpx != NULL) {
			v = lpx->p->y;
			h = lpx->p->x;
			dst[v*width+h] = 0;	// region�� ������
			lpx_for_free = lpx;
			lpx = lpx->next;
			free(lpx_for_free);
			lpx_for_free = NULL;
		}
		lrg_for_free = lrg;
		lrg = lrg->next;
		free(lrg_for_free);
		lrg_for_free = NULL;
	}

	lpx = border;
	while (lpx != NULL) {
		v = lpx->p->y;
		h = lpx->p->x;
		dst[v*width+h] = 255;	// border�� ���
		lpx_for_free = lpx;
		lpx = lpx->next;
		free(lpx_for_free);
		lpx_for_free = NULL;
	}

	root_region = NULL;
	border = NULL;

	return 0;
}