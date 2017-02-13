/*
 * Copyright (c) 2008 Intel Corporation
 * Author: Matthew Wilcox <willy@linux.intel.com>
 *
 * Distributed under the terms of the GNU GPL, version 2
 *
 * Please see kernel/semaphore.c for documentation of these functions
 */
#ifndef __LINUX_SEMAPHORE_H
#define __LINUX_SEMAPHORE_H

#include <linux/list.h>
#include <linux/spinlock.h>

/* Please don't access any members of this structure directly */
/*
 * ����lock�Ǹ�����������,����ʵ�ֶ�count��ԭ�Ӳ���
 * count���ڱ�ʾͨ�����ź������������ٽ�����ִ��·���ĸ���
 * wait_list���ڹ��������ڸ��ź�����˯�ߵĽ��̣�
          �޷���ø��ź����Ľ��̽�����˯��״̬
 */
struct semaphore {
	raw_spinlock_t		lock;
	unsigned int		count;
	struct list_head	wait_list;
};

/*����ź����ĳ�ʼ��*/
#define __SEMAPHORE_INITIALIZER(name, n)				\
{									\
	.lock		= __RAW_SPIN_LOCK_UNLOCKED((name).lock),	\
	.count		= n,						\
	.wait_list	= LIST_HEAD_INIT((name).wait_list),		\
}

/*countֵΪ1,ʵ�ֻ������,����ʱ��ֻ����һ�����̽����ٽ���*/
#define DEFINE_SEMAPHORE(name)	\
	struct semaphore name = __SEMAPHORE_INITIALIZER(name, 1)

/*��ʼ���ź���*/
static inline void sema_init(struct semaphore *sem, int val)
{
	static struct lock_class_key __key;
	/*��ʼ����Ҫ����*/
	*sem = (struct semaphore) __SEMAPHORE_INITIALIZER(*sem, val);
	lockdep_init_map(&sem->lock.dep_map, "semaphore->lock", &__key, 0);
}

/*��down_interruptible��ȣ�down�ǲ����жϵ�*/
extern void down(struct semaphore *sem);
/*���жϵ�*/
extern int __must_check down_interruptible(struct semaphore *sem);
/*˯�ߵĽ��̿������յ�һЩ�������źű����Ѷ����»�ȡ�ź����Ĳ���ʧ�ܣ�����ʹ��*/
extern int __must_check down_killable(struct semaphore *sem);
/*������ͼ����ź��������޷����ֱ�ӷ��أ���������˯��״̬��
���أ���ζ�ź����ĵ������Ѿ�������ź���*/
extern int __must_check down_trylock(struct semaphore *sem);
/* �������޷�����ź���������½�����˯��״̬�����Ǵ�������˯��״̬��ʵ������
 * ���jiffiesָ����ʵ�ʵ���ʱ������Ȼ�޷�����ź������򽫷��ش�����-ETIME��
 * �ڵ���ǰ���̵�˯��״̬ΪTASK_UNINTERRUPTIBLE���ɹ�����ź�������0.*/
extern int __must_check down_timeout(struct semaphore *sem, long jiffies);
/*ֻ��һ��UP����*/
extern void up(struct semaphore *sem);

#endif /* __LINUX_SEMAPHORE_H */