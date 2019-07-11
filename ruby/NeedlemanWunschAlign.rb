# https://en.wikipedia.org/wiki/Needleman%E2%80%93Wunsch_algorithm

class NeedlemanWunschAlign

    def self.align(a, b)
        table = self.create_table(a, b)

        # derive only 1 candidate
        r = a.length
        c = b.length
        aling_a = ""
        aling_b = ""

        while r > 0 or c > 0 do
            s = a[r-1] == b[c-1] ? 1 : -1

            if r > 0 and c > 0 and table[r][c] == table[r-1][c-1] + s
                aling_a+=(a[r-1])
                aling_b+=(b[c-1])
                r -= 1
                c -= 1
                next
            end

            if r > 0 and table[r][c] == table[r-1][c] - 1
                aling_a+=(a[r-1])
                aling_b+=("_")
                r -= 1
                next
            end

            aling_b+=(b[c-1])
            aling_a+=("_")
            c -= 1
        end

        return [aling_a.reverse, aling_b.reverse, table[a.length][b.length]]
    end


    include Enumerable

    def initialize(a, b)
        @a = a
        @b = b
    end

    def each(&block)
        table = NeedlemanWunschAlign.create_table(@a, @b)
        iterate(@a.length, @b.length, table, "", "", &block)
    end

    private

        def iterate(r, c, table, aling_a, aling_b, &block)
            if r == 0 and c == 0
                yield [aling_a.reverse, aling_b.reverse, table[@a.length][@b.length]]
                return
            end

            s = @a[r-1] == @b[c-1] ? 1 : -1

            if r > 0 and c > 0 and table[r][c] == table[r-1][c-1] + s
                iterate(r-1, c-1, table, aling_a+@a[r-1], aling_b+@b[c-1], &block)
            end

            if r > 0 and table[r][c] == table[r-1][c] - 1
                # aling_a+=(a[r-1])
                # aling_b+=("_")
                # r -= 1
                # next
                iterate(r-1, c, table, aling_a+@a[r-1], aling_b+"_", &block)
            end

            if c > 0 and table[r][c] == table[r][c-1] - 1
                # aling_b+=(b[c-1])
                # aling_a+=("_")
                # c -= 1
                iterate(r,c-1,table, aling_a+"_", aling_b+@b[c-1], &block)
            end

            return
        end

        def self.create_table(a, b)
            table = Array.new(a.length + 1).map{ Array.new(b.length + 1, 0) }

            # set upper row and left coloum
            table.each_with_index do |row, i|
                row[0] = -i
            end
            table[0].length.times do |i|
                table[0][i] = -i
            end

            a.length.times do |j|
                b.length.times do |i|

                    score_to_bottom = table[j][i + 1] - 1
                    score_to_right = table[j + 1][i] - 1
                    score_to_diagonal = table[j][i] - 1
                    score_to_diagonal += 2 if a[j] == b[i]

                    score = [score_to_bottom, score_to_right, score_to_diagonal].max
                    table[j + 1][i + 1] = score
                end
            end

            return table
        end

end

if $0 == __FILE__
    p("start")
    p NeedlemanWunschAlign.align("GATTACA","GCATGCU")
    p NeedlemanWunschAlign.align("abdce", "abdce")
end