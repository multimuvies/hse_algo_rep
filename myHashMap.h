#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <list>
#include <cmath>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {

public:
    using iterator = typename std::list<std::pair<const KeyType, ValueType>>::iterator;
    using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;

private:
    size_t size_ = 0;

    struct bucket {
        iterator bucket_iter;
        bool used = false;
        size_t psl = 0;
        bucket(): used(0), psl(0){}
    };

    std::list<std::pair<const KeyType, ValueType>> data_;
    Hash hasher_;
    size_t M = 16;
    std::vector<bucket> buckets = std::vector<bucket>(M);

public:
    Hash hash_function() const {
        return hasher_;
    }

    size_t find_pos(KeyType key) const {
        size_t pos = hasher_(key)%M;
        size_t cur_psl = 0;
        for (size_t i = 0; i < M; ++i) {
            size_t j = (pos+i)%M;
            if (buckets[j].used == 0 || cur_psl > buckets[j].psl)
                break;

            if (buckets[j].bucket_iter->first == key)
                return j;
            cur_psl++;
        }
        return M;
    }

    iterator begin() {
        return data_.begin();
    }

    const_iterator begin() const {
        return data_.begin();
    }

    iterator end() {
        return data_.end();
    }

    const_iterator end() const {
        return data_.end();
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void clear() {
        std::vector<size_t> element_positions;
        for (auto i: data_) {
            size_t p1 = find_pos(i.first);
            element_positions.push_back(p1);
        }
        for (auto u: element_positions) {
            buckets[u].used = 0;
            buckets[u].psl = 0;
        }
        size_ = 0;
        data_.clear();
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list, Hash _hash_f = Hash()) : hasher_(_hash_f) {
        for (auto i: list) {
            insert(i);
        }
    }

    HashMap(Hash _hash_f = Hash()) : hasher_(_hash_f) {
        buckets.resize(M, bucket());
    }

    template<typename IterType>
    HashMap(IterType begin, IterType end, Hash _hash_f = Hash()) : hasher_(_hash_f) {
        while (begin != end) {
            insert(*begin);
            ++begin;
        }
    }

    const HashMap &operator=(const HashMap &other) {
        clear();
        for (auto i: other) {
            this->insert(i);
        }
        return other;
    }

    ValueType &operator[](const KeyType key) {
        iterator it = this->find(key);
        if (it == this->end()) {
            this->insert(std::make_pair(key, ValueType()));
            it = this->find(key);
        }
        return it->second;
    }

    const ValueType &at(const KeyType key) const {
        const_iterator it = this->find(key);
        if (it == this->end()) {
            throw std::out_of_range("");
        }
        return it->second;
    }

    iterator find(KeyType key){
        size_t i = this->find_pos(key);
        if (i == M)
            return end();
        return buckets[i].bucket_iter;
    }

    const_iterator find(KeyType key) const {
        size_t i = this->find_pos(key);
        if (i == M)
            return end();
        return buckets[i].bucket_iter;
    }

    void rehash() {
        if (size_ * 2 >= M) {
            M *= 2;
            buckets.clear();
            buckets.resize(M);
            std::list<std::pair<const KeyType, ValueType>> element_positions;
            for (auto i: data_) {
                element_positions.push_back(i);
            }
            data_.clear();
            size_ = 0;
            for (auto i: element_positions) {
                insert(i);
            }
        }
    }

    iterator insert(std::pair<const KeyType, ValueType> pr) {
        rehash();
        if (find(pr.first) != end())//already exists
        {
            return find(pr.first);
        } else {
            size_t pos = hasher_(pr.first) % M;
            bucket cur_bucket;
            cur_bucket.used = 1;
            size_++;
            cur_bucket.psl = 0;
            data_.push_back(pr);
            cur_bucket.bucket_iter = data_.end();
            --cur_bucket.bucket_iter;
            for (size_t i = 0; i < M; ++i) {
                size_t j = (i + pos) % M;
                if (buckets[j].used == 0) {
                    buckets[j] = cur_bucket;
                    break;
                }
                if (cur_bucket.psl > buckets[j].psl) {
                    std::swap(buckets[j], cur_bucket);
                }
                cur_bucket.psl++;
            }
            return (--data_.end());
        }
    }

    void erase(KeyType key) {
        size_t pos = find_pos(key);
        if (pos == M) {
            return;
        } else {
            buckets[pos].used = 0;
            buckets[pos].psl = 0;
            data_.erase(buckets[pos].bucket_iter);
            size_--;
            for (size_t i = 0; i < M; ++i) {
                size_t j1 = (i + pos) % M;
                size_t j2 = (i + pos + 1) % M;
                if (buckets[j2].used == 0 || buckets[j2].psl == 0)
                    break;
                else {
                    std::swap(buckets[j1], buckets[j2]);
                    buckets[j1].psl--;
                }
            }
        }
    }
};
