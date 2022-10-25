// Section 1.8: Problems 1, 4 (1/10626), 6 (2/n), 10 (0.1630), 11, 12 
// (0.5798), 17

// Section 1.5: 3: Consider two events A and B such that Pr(A) = 1/3 and Pr(B) = 1/2. Determine the value of Pr(B ∩ A^c) for each of the following conditions:
//             (a) A and B are disjoint;
//             (b) A ⊂ B;
//             (c) Pr(A ∩ B) = 1/8.

//             (a): Pr(A) = 1/3, Pr(A^c) = 2/3. ∵A and B are disjoint. ∴Pr(B ∩ A^c) = Pr(B) = 1/2.
//             (b): Pr(B ∩ A^c) = 1/2 - 1/3 = 1/6.
//             (c): Pr(B ∩ A^c) = 1/2 - 1/8 = 3/8.

//             4: If the probability that student A will fail a certain statistics examination is 0.5, the probability that student B will fail the examination is 0.2, and the probability that both student A and 
//             student B will fail the examination is 0.1, what is the probability that at least one of these two students will fail the examination?

//             0.5 + 0.2 - 0.1 = 0.6

//             6: For the conditions of Exercise 4, what is the probability that exactly one of the two students will fail the examination?

//             (0.5 - 0.1) + (0.2 - 0.1) = 0.5

//             7: Consider two events A and B with Pr(A) = 0.4 andnPr(B) = 0.7. Determine the maximum and minimum possible values of Pr(A ∩ B) and the conditions under which each of these values is attained.

//             Max: When A ⊂ B,  Pr(A ∩ B) = 0.4
//             Min: When A U B = 1, Pr(A ∩ B) = 0.1
            
//             9: Prove that for every two events A andB, the probability that exactly one of the two events will occur is given by the expression Pr(A) + Pr(B) - 2 Pr(A ∩ B).

//             Use graph to prove it.

//             14: Consider, once again, the four blood types A, B, AB, and O described in Exercise 8 in Sec. 1.4 together with the two antigens anti-A and anti-B. Suppose that, for a given person, the probability of 
//             type O blood is 0.5, the probability of type A blood is 0.34, and the probability of type B blood is 0.12.
//             (a): Find the probability that each of the antigens will react with this person's blood.
//             (b): Find the probability that both antigens will react with this person's blood.

//             O: 0.5, B: 0.12, A: 0.34, AB: 0.04
//             (a): A: 0.34 + 0.04 = 0.38, B: 0.12 + 0.04 = 0.16
//             (b): AB: 0.04

// Section 1.6: 1: If two balanced dice are rolled, what is the probability that the sum of the two
//                 numbers that appear will be odd?

//             3*6 / 6*6 = 1/2

//             3: If two balanced dice are rolled, what is the probability that the difference 
//             between the two numbers that appear will be less than 3?

//             3 + 4 + 5 + 5 + 4 + 3/ 6*6 = 24 / 36 = 2/3

//             4: A school contains students in grades 1, 2, 3, 4, 5, and 6. Grades 2, 3, 4, 5, 
//             and 6 all contain the same number of students, but there are twice this number in 
//             grade 1. If a student is selected at random from a list of all the students in the 
//             school, what is the probability that she will be in grade 3?

//             1/7

//             6: If three fair coins are tossed, what is the probability that all three faces will
//             be the same?

//             (1/2)^3 = 1/8
//             2 * (1/8) = 1/4

// Section 1.7: 2: 20 x 18 x 25 = 9000

//             3: 5! = 120

//             5: The probability for choose four number in six numbers on dice: 6C4
//                 The probability for four different number: 4!
//                 Answer: 6C4 x 4!

//             7. favorable / total = 20 x 19 x 18...x 10 x 9 / 20^12

//             8. 7 x 6 x 5 x 4 x 3 / 7^5

fn main() {
    println!("Hello, world!");
    another_function();
}

fn another_function() {
    println!("!dlrow olleH");
}