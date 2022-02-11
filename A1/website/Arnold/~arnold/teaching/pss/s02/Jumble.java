import java.io.*;
import java.util.*;
public class Jumble {

	public static void main(String [] args) throws Exception {
		solveProblem();
	}

	public static void solveProblem() throws Exception {
		/*
		maintain a set s 
		each member of the set consists of jumble equivalent words
	
		for each word w in the dictionary
			check if w is a jumble of any member of the set
			if so then add w to this set
			otherwise create a new set with w as its only member
	
		the largest member of s is the solution
		*/
		Vector v=new Vector(); // Members of v will themselves be Vectors
		BufferedReader br=new BufferedReader(new FileReader("words"));
		String line;
		int i=0;
		while(true){
			i++;
			if(i%100==0)System.out.println(i);
			line=br.readLine();
			if(line==null)break;
			addWord(v,line);
		}
		printLists(v);
	}
	// Add w to the list that it belongs in
	public static void addWord(Vector v, String w){
		findWordList(v,w).add(w);
	}

	// Find the list in v that w belongs in 
	public static Vector findWordList(Vector v,String w){
		Enumeration e=v.elements();
		while(e.hasMoreElements()){
			Vector list=(Vector)e.nextElement();
			if(isJumble((String)list.get(0), w))return list;
		}
		// If we have not found the list that w belongs in
		// then it belongs in a new list
		Vector list=new Vector();
		v.add(list);
		return list;
	}

	/* Determine if w1 and w2 are jumbles of eachother */
	public static boolean isJumble(String w1, String w2){
		return isJumbleHelper(w1.toLowerCase(), w2.toLowerCase());
	}
	
	public static boolean isJumbleHelper(String w1, String w2){
	/* w1 is a jumble of w2 if
	 * 	w1 and w2 are both empty
	 * 	w1 and w2 have the same length 
	 * 		and the first character in w1 appears in w2
	 * 		and the remains of w1 and w2 (when the first
	 *		character is deleted) are jumbles of eachother
	 */
	
	/*
	 *	if w1.length!=w2.length return false
	 *	if w1==w2=="" then return true
	 *	let c=the first character in w1
	 *	if c appears in w2 then
	 *		newW2=remove c from w2
	 *		newW1=remove c from w1
	 *		return isJumble(newW1,newW2)
	 *	else return false
	 */
		if(w1.length()!=w2.length())return false;
		if(w1.length()==0 && w2.length()==0)return true;
		char c=w1.charAt(0);
		int pos=w2.indexOf(c);
		if(pos==-1)return false;
		String newW2=w2.substring(0,pos)+w2.substring(pos+1);
		String newW1=w1.substring(1);
		return isJumbleHelper(newW1, newW2);
	}

	public static void printLists(Vector v){
		Enumeration e=v.elements();
		while(e.hasMoreElements()){
			Vector list=(Vector)e.nextElement();
			System.out.println(list);
		}
	}
}
