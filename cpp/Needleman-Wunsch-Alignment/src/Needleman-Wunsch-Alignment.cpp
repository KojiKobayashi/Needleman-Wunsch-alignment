﻿#include "Needleman-Wunsch-Alignment.h"
#include <algorithm>
#include <string>
#include <list>
#include <stdexcept>

namespace Alignment
{
    AlignResults::AlignResults(const int score)
    {
        this->score = score;
        this->strings = std::vector<AlignedStrings>();
    }

    AlignResults::AlignResults(std::vector<AlignedStrings>& strings, const int score)
    {
        this->score = score;
        this->strings = std::vector<AlignedStrings>{ strings };
    }

    void AlignResults::AddStrings(AlignedStrings& strings)
    {
        this->strings.push_back(strings);
    }

    int AlignResults::GetScore() const
    {
        return score;
    }

    int AlignResults::GetCandidateCount() const
    {
        return static_cast<int>(strings.size());
    }

    AlignedStrings AlignResults::GetStrings(const int candidateIndex) const
    {
        auto tmp = strings.at(candidateIndex);
        return tmp;
    }

    class Aligner
    {
    private:
        std::string first;
        std::string second;
        char separator;
        bool onlyOnePair;
        int* rawTable = nullptr;
        int** table = nullptr;

    public:
        Aligner(std::string& first, std::string& second, const char separator, const bool onlyOnePair = false)
        {
            this->first = first;
            this->second = second;
            this->separator = separator;
            this->onlyOnePair = onlyOnePair;
        }

        AlignResults Align()
        {
            std::list<AlignedStrings> stringsList;
            const auto r = first.length();
            const auto c = second.length();
            const std::string alignA, alignB;

            try
            {
                CreateTable();
                AlignResults results(table[r][c]);
                Iterate(r, c, alignA, alignB, results);

                DeleteTable();
                return results;
            }
            catch (...)
            {
                DeleteTable();
                throw;
            }
        }

    private:
        void CreateTable()
        {
            // create table
            const auto rowSize = static_cast<long long>(first.size()) + 1;
            const auto colSize = static_cast<long long>(second.size()) + 1;

            rawTable = new int[colSize * rowSize];
            table = new int* [rowSize];
            for (auto i = 0; i < rowSize; i++)
                table[i] = rawTable + i * colSize;

            for (auto i = 0; i < colSize; i++)
                table[0][i] = -i;
            for (auto i = 0; i < rowSize; i++)
                table[i][0] = -i;

            for (auto j = 0; j < rowSize - 1; j++)
            {
                for (auto i = 0; i < colSize - 1; i++)
                {
                    auto scoreBottom = table[j][i + 1] - 1;
                    auto scoreRight = table[j + 1][i] - 1;
                    auto scoreDiagonal = table[j][i] - 1;
                    if (first[j] == second[i])
                        scoreDiagonal += 2;
                    table[j + 1][i + 1] = std::max(scoreBottom, std::max(scoreRight, scoreDiagonal));
                }
            }
        }

        void DeleteTable()
        {
            delete[] rawTable;
            delete[] table;
        }

        bool Iterate(const size_t r, const size_t c, std::string alignA, std::string alignB, AlignResults& results)
        {
            if (r == 0 && c == 0)
            {
                std::reverse(alignA.begin(), alignA.end());
                std::reverse(alignB.begin(), alignB.end());
                auto tmp = AlignedStrings{ alignA, alignB };
                results.AddStrings(tmp);
                if (onlyOnePair)
                    return true;
                else
                    return false;
            }

            if (r > 0 && c > 0)
            {
                const auto s = first[r - 1] == second[c - 1] ? 1 : -1;
                if (table[r][c] == table[r - 1][c - 1] + s)
                {
                    if (Iterate(r - 1, c - 1, alignA + first[r - 1], alignB + second[c - 1], results))
                        return true;
                }
            }

            if (r > 0 && table[r][c] == table[r - 1][c] - 1)
            {
                if (Iterate(r - 1, c, alignA + first[r - 1], alignB + separator, results))
                    return true;
            }

            if (c > 0 && table[r][c] == table[r][c - 1] - 1)
            {
                if (Iterate(r, c - 1, alignA + separator, alignB + second[c - 1], results))
                    return true;
            }

            return false;
        }
    };

    AlignResults NeedlemanWunschAlignment::Align(std::string first, std::string second, const char separator)
    {
        if (first.find(separator) != std::string::npos || second.find(separator) != std::string::npos)
            throw std::invalid_argument("string contains the separator.");

        Aligner i(first, second, separator);
        return i.Align();
    }

    AlignResults NeedlemanWunschAlignment::AlignOnePair(std::string first, std::string second, const char separator)
    {
        if (first.find(separator) != std::string::npos || second.find(separator) != std::string::npos)
            throw std::invalid_argument("string contains the separator.");

        Aligner i(first, second, separator, true);
        return i.Align();
    }
}
