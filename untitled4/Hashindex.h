
#ifndef PROJECTBD_HASHINDEX_H
#define PROJECTBD_HASHINDEX_H

#include <iostream>
#include <functional>
#include <bitset>
#include "SMH.h"
#include "Bucket.h"
#include "record.h"
using namespace std;

struct TreeNode {
    int start{};
    int end{};
    int bucketIndex;
    bool isLeaf;
    int nextDeleted;
    int next_pos;

    TreeNode() = default;

    TreeNode(int left, int right) {
        end = right;
        start = left;
        isLeaf = false;
        nextDeleted = 0;

    }

    TreeNode(int bucketindex) {
        bucketIndex = bucketindex;
        isLeaf = true;
        nextDeleted = 0;
    }

    int getBucketIndex() {
        if (isLeaf) {
            return bucketIndex;
        }
        return -1;
    }
};

template <class Record, class Key, class Hash = hash<Key>>
class HashIndex {
private:
    int MaxHeight;
    typedef Bucket<Record, Key> Bucketcito;
    SMH<TreeNode> IndexData;
    typedef bitset<50> BitsetData;
    SMH<Bucketcito> BucketData;
    Hash myHash;
    int accessMemSec = 0;

public:
    HashIndex() = default;

    HashIndex(const string& File, const string& FileBucket, int max = 3) {
        BucketData.setFilename(FileBucket);
        IndexData.setFilename(File);
        MaxHeight = max;
    }

    void initialize() {
        TreeNode root(1, 2), v1(0), v2(1);
        Bucketcito b1; Bucketcito b2;
        BucketData.add(b1); BucketData.add(b2); IndexData.add(root);
        IndexData.add(v1); IndexData.add(v2);
    }

    int searchNode(int nodePosition, BitsetData key, int& height) {
        TreeNode node = IndexData.readRecord(nodePosition);
        accessMemSec++;
        if (node.isLeaf) {
            return nodePosition;
        }
        else {
            if (key[height] == 0) {
                return searchNode(node.start, key, ++height);
            }
            else return searchNode(node.end, key, ++height);
        }
    }

    vector<Record> search(Key searchKey) {
        BitsetData hashKey = myHash(searchKey);
        int height = 0;
        int currentNodePosition = searchNode(0, hashKey, height);
        TreeNode currentNode = IndexData.readRecord(currentNodePosition);
        accessMemSec++;
        int currentBucketPosition = currentNode.getBucketIndex();
        vector<Record> output;
        while (currentBucketPosition != -1) {
            Bucketcito bucket = BucketData.readRecord(currentBucketPosition);
            accessMemSec++;
            for (auto& r : bucket.getRecords()) {
                if (r.equalToKey(searchKey)) {
                    output.push_back(r);
                }
            }
            currentBucketPosition = bucket.getNextBucket();
        }
        return output;
    }

    void insert(Record record) {
        if (BucketData.getNumRecords() == 0) { initialize(); }
        int height = 0;
        BitsetData hashKey = myHash(record.getPrimaryKey());
        int currentNodePosition = searchNode(0, hashKey, height);
        TreeNode currentNode = IndexData.readRecord(currentNodePosition);
        accessMemSec++;
        int currentBucketPosition = currentNode.getBucketIndex();
        Bucketcito bucket = BucketData.readRecord(currentBucketPosition);
        accessMemSec++;
        if (!bucket.isFull()) {
            bucket.add(record);
            bucket.sortBucket();
            BucketData.writeRecord(currentBucketPosition, bucket);
            accessMemSec++;
        }
        else {
            split(record, height, currentNodePosition, currentNode, currentBucketPosition, bucket);
        }
    }

    void split(Record& record, int height, int currentNodePosition, TreeNode& currentNode, int currentBucketPosition, Bucketcito& bucket) {
        if (height == MaxHeight) {
            Bucketcito bucketToLink;
            bucketToLink.add(record);
            bucketToLink.setNextBucket(currentBucketPosition);
            currentNode.bucketIndex = BucketData.add(bucketToLink);
            IndexData.writeRecord(currentNodePosition, currentNode);
            accessMemSec++;
            return;
        }
        vector<Record> records = bucket.getRecords();
        records.push_back(record);
        Bucketcito bucket1, bucket2;
        bool splitAgain = false;
        bool splitToLeft = false;
        for (auto& r : records) {
            BitsetData hashKey = myHash(r.getPrimaryKey());
            if (hashKey[height] == 0) {
                if (!bucket1.isFull()) {
                    bucket1.add(r);
                }
                else {
                    splitAgain = true;
                    splitToLeft = true;
                }
            }
            else {
                if (!bucket2.isFull()) bucket2.add(r);
                else splitAgain = true;
            }
        }
        BucketData.deleteRecord(currentBucketPosition);
        bucket1.sortBucket();
        bucket2.sortBucket();
        int bucketPosition1 = BucketData.add(bucket1);
        int bucketPosition2 = BucketData.add(bucket2);
        TreeNode leftNode(bucketPosition1), rightNode(bucketPosition2);
        currentNode.start = IndexData.add(leftNode);
        currentNode.end = IndexData.add(rightNode);
        currentNode.isLeaf = false;
        IndexData.writeRecord(currentNodePosition, currentNode);
        accessMemSec++;
        if (splitAgain) {
            int splitNodePosition, splitBucketPosition;
            TreeNode splitNode;
            if (splitToLeft) {
                splitNodePosition = currentNode.start;
                splitNode = IndexData.readRecord(splitNodePosition);
                accessMemSec++;
                split(record, height + 1, splitNodePosition, splitNode, bucketPosition1, bucket1);
            }
            else {
                splitNodePosition = currentNode.end;
                splitNode = IndexData.readRecord(splitNodePosition);
                accessMemSec++;
                split(record, height + 1, splitNodePosition, splitNode, bucketPosition2, bucket2);
            }
        }
    }

    void remove(Key key) {
        int height = 0;
        removeUtil(0, key, myHash(key), height);
    }

    void removeUtil(int nodePosition, Key key, BitsetData hashKey, int& height) {
        TreeNode currentNode = IndexData.readRecord(nodePosition);
        accessMemSec++;
        if (currentNode.isLeaf) {
            deleteRecordInBucket(key, currentNode, nodePosition);
            return;
        }
        if (0 == hashKey[height]) {
            removeUtil(currentNode.start, key, hashKey, ++height);
        }
        else {
            removeUtil(currentNode.end, key, hashKey, ++height);
        }

        if (nodePosition == 0) {
            return;
        }
        TreeNode left = IndexData.readRecord(currentNode.start);
        accessMemSec++;
        TreeNode right = IndexData.readRecord(currentNode.end);
        accessMemSec++;

        if (left.isLeaf && right.isLeaf) {
            Bucketcito bucket1 = BucketData.readRecord(left.bucketIndex);
            accessMemSec++;
            Bucketcito bucket2 = BucketData.readRecord(right.bucketIndex);
            accessMemSec++;
            if (bucket1.empty() && bucket2.empty()) {
                BucketData.deleteRecord(left.bucketIndex);
                BucketData.deleteRecord(right.bucketIndex);
                IndexData.deleteRecord(currentNode.start);
                IndexData.deleteRecord(currentNode.end);
                Bucketcito newBucket;
                currentNode.isLeaf = true;
                currentNode.bucketIndex = BucketData.add(newBucket);
                IndexData.writeRecord(nodePosition, currentNode);
                accessMemSec++;
            }
        }
    }

    void deleteRecordInBucket(Key key, TreeNode &currentNode, int nodePosition) {
        int currentBucketPosition = currentNode.bucketIndex;
        Bucketcito bucket = BucketData.readRecord(currentBucketPosition);
        accessMemSec++;
        auto differentRecords = bucket.getAllDifferentRecords(key);
        bucket.setRecords(differentRecords);
        BucketData.writeRecord(currentBucketPosition, bucket);
        accessMemSec++;
        if (bucket.getNextBucket() != -1 && bucket.empty()) {
            currentNode.bucketIndex = bucket.getNextBucket();
            BucketData.deleteRecord(currentBucketPosition);
            IndexData.writeRecord(nodePosition, currentNode);
            accessMemSec++;
            deleteRecordInBucket(key, currentNode, nodePosition);
        }
    }

    int getNumberAccess() {
        return accessMemSec;
    }

    void restartNumberAccess() {
        accessMemSec = 0;
    }
};

#endif //PROJECTBD_HASHINDEX_H